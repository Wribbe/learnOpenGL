#version 330 core

out vec4 color;

in vec3 Normal;
in vec3 fragment_position;
in vec2 texture_coordinates;

uniform vec3 view_position;
uniform vec3 camera_front;
uniform int POINT_LIGHTS;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Direct_Light {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Point_Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float constant;
  float linear;
  float quadratic;
};

#define LIGHTS 4

uniform Material material;
uniform Direct_Light direct_light;
uniform Point_Light point_light;

uniform Point_Light point_lights[LIGHTS];

vec3 get_direct_light(Direct_Light light, vec3 normal, vec3 view_direction);
vec3 get_point_light(Point_Light light, vec3 normal, vec3 fragment_position,
                 vec3 view_direction);

void main() {

  // Properties.
  vec3 normal = normalize(Normal);
  vec3 view_direction = normalize(view_position - fragment_position);

  vec3 result = vec3(0.0f);
  // Phase 1: Directional lighting.
  //result = get_direct_light(direct_light, normal, view_direction);
  // Phase 2: Point lights.
  int i;
  for(i=0; i < LIGHTS; i++) {
    result += get_point_light(point_lights[i], normal, fragment_position,
                              view_direction);
  }
  //result = point_lights[1].ambient;
  result = normal;
  color = vec4(result, 1.0);
}

//  vec3 ambient = light.ambient * vec3(texture(material.diffuse,
//                                      texture_coordinates));
//  vec3 norm = normalize(Normal);
//  vec3 light_distance_vector = view_position - Frag_position;
//
//  vec3 light_direction = normalize(light_distance_vector);
//  //vec3 light_direction = normalize(-light.direction);
//
//
//  vec3 view_direction = normalize(view_position - Frag_position);
//  float diff = max(dot(norm, light_direction), 0.0f);
//
//  //float falloff = 1.0f/dot(light_distance_vector, light_distance_vector);
//  float squared_distance = dot(light_distance_vector, light_distance_vector);
//  float light_distance = sqrt(squared_distance);
//
//  float linear_falloff = light.linear * light_distance;
//  float quadratic_falloff = light.quadratic * squared_distance;
//  float constant_falloff = light.constant;
//
//  float falloff = 1.0f/(linear_falloff + quadratic_falloff + constant_falloff);
//
//  float theta = dot(normalize(view_direction), normalize(-camera_front));
//  float epsilon = light.cutoff_angle - light.outer_cutoff_angle;
//  float intensity = clamp((theta - light.outer_cutoff_angle) / epsilon, 0.0,
//  1.0f);
//
//  vec3 diffuse_map = vec3(texture(material.diffuse, texture_coordinates));
//  vec3 diffuse = (light.diffuse * (diff * diffuse_map));
//
//  vec3 reflect_direction = reflect(-light_direction, norm);
//  float specular_value = pow(max(dot(view_direction, reflect_direction), 0.0f),
//                                     material.shininess);
//
//  vec3 specular_map = vec3(texture(material.specular, texture_coordinates));
//  vec3 specular = (light.specular * specular_value * specular_map);
//
//  specular *= falloff;
//  diffuse *= falloff;
//  ambient *= falloff;
//
//  diffuse *= intensity;
//  specular *= intensity;
//
//  vec3 result;
//  result = ambient + diffuse + specular;
//  color = vec4(result, 1.0f);

vec3 get_direct_light(Direct_Light light, vec3 normal, vec3 view_direction) {
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

vec3 get_point_light(Point_Light light, vec3 normal, vec3 fragment_position,
                 vec3 view_direction) {
  vec3 light_vector = light.position - fragment_position;
  vec3 light_direction = normalize(light_vector);
  // Diffuse shading.
  float diffuse_factor = max(dot(normal, light_direction), 0.0);
  // Specular shading.
  vec3 reflect_direction = reflect(-light_direction, normal);
  float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0),
                              material.shininess);
  // Attenuation
  float squared_light_distance = dot(light_vector, light_vector);
  float light_distance = sqrt(squared_light_distance);

  float falloff_constant, falloff_linear, falloff_quadratic;

  falloff_constant = 1.0f;
  falloff_linear = light.linear * light_distance;
  falloff_quadratic = light.quadratic * squared_light_distance;

  float attenuation = 1.0f / (falloff_constant + falloff_linear +
                              falloff_quadratic);

  vec3 diffuse_sampling = vec3(texture(material.diffuse, texture_coordinates));
  vec3 specular_sampling = vec3(texture(material.specular, texture_coordinates));

  vec3 ambient = light.ambient * diffuse_sampling;
  vec3 diffuse = light.diffuse * diffuse_factor * diffuse_sampling;
  vec3 specular = light.specular * specular_factor * specular_sampling;

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}
