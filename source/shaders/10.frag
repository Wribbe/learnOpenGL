#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 Frag_position;
in vec2 texture_coordinates;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;
uniform vec3 camera_front;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {
  // Directional light.
  vec3 direction;

  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  // Point light.
  float constant;
  float linear;
  float quadratic;

  // Spotlight.
  float cutoff_angle;
  float outer_cutoff_angle;
};

struct Direct_Light {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular
}


uniform Material material;
uniform Light light;
uniform Direct_Light direct_light;

vec3 direct_light(Direct_Light light, vec3 normal, vec3 view_direction);

void main() {

  vec3 ambient = light.ambient * vec3(texture(material.diffuse,
                                      texture_coordinates));
  vec3 norm = normalize(Normal);
  vec3 light_distance_vector = view_position - Frag_position;

  vec3 light_direction = normalize(light_distance_vector);
  //vec3 light_direction = normalize(-light.direction);


  vec3 view_direction = normalize(view_position - Frag_position);
  float diff = max(dot(norm, light_direction), 0.0f);

  //float falloff = 1.0f/dot(light_distance_vector, light_distance_vector);
  float squared_distance = dot(light_distance_vector, light_distance_vector);
  float light_distance = sqrt(squared_distance);

  float linear_falloff = light.linear * light_distance;
  float quadratic_falloff = light.quadratic * squared_distance;
  float constant_falloff = light.constant;

  float falloff = 1.0f/(linear_falloff + quadratic_falloff + constant_falloff);

  float theta = dot(normalize(view_direction), normalize(-camera_front));
  float epsilon = light.cutoff_angle - light.outer_cutoff_angle;
  float intensity = clamp((theta - light.outer_cutoff_angle) / epsilon, 0.0,
  1.0f);

  vec3 diffuse_map = vec3(texture(material.diffuse, texture_coordinates));
  vec3 diffuse = (light.diffuse * (diff * diffuse_map));

  vec3 reflect_direction = reflect(-light_direction, norm);
  float specular_value = pow(max(dot(view_direction, reflect_direction), 0.0f),
                                     material.shininess);

  vec3 specular_map = vec3(texture(material.specular, texture_coordinates));
  vec3 specular = (light.specular * specular_value * specular_map);

  specular *= falloff;
  diffuse *= falloff;
  ambient *= falloff;

  diffuse *= intensity;
  specular *= intensity;

  vec3 result;
  result = ambient + diffuse + specular;
  color = vec4(result, 1.0f);
}

vec3 direct_light(Direct_Light light, vec3 normal, vec3 view_direction) {
  vec3 light_direction = normalize(-light.direction);
  // Diffuse shading.
  float diffuse_factor = max(dot(normal, light_direction), 0.0);
  // Specular shading.
  vec3 reflection_direction = reflect(-light_direction, normal);
  float specular_factor = pow(max(dot(view_direction, reflection_direction),
                                  0.0), material.shininess);
  // Combine results.
  vec3 ambient, diffuse, specular;
  vec3 diffuse_map, specular_map;

  diffuse_map = vec3(texture(material.diffuse, texture_coordinates));
  specular_map = vec3(texture(material.specular, texture_coordinates));

  ambient = light.ambient * diffuse_map;
  diffuse = light.diffuse * diffuse_factor * diffuse_map;
  specular = light.specular * specular_factor * specular_map;
  return ambient + diffuse + specular;
}
