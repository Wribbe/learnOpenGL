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

vec4 get_direct_light(Direct_Light light, vec3 normal, vec3 view_direction);
vec4 get_point_light(Point_Light light, vec3 normal, vec3 fragment_position,
                 vec3 view_direction);

void main() {

  vec4 diffuse_sampling = texture(material.diffuse, texture_coordinates);
  if (diffuse_sampling.a < 0.1) {
    discard;
  }

  // Properties.
  vec3 normal = normalize(Normal);
  vec3 view_direction = normalize(view_position - fragment_position);

  vec4 result = vec4(0.0f);
  // Phase 1: Directional lighting.
  //result = get_direct_light(direct_light, normal, view_direction);
  // Phase 2: Point lights.
  int i;
  for(i=0; i < LIGHTS; i++) {
    result += get_point_light(point_lights[i], normal, fragment_position,
                              view_direction);
  }
  //result = point_lights[1].ambient;
  color = result;
}

vec4 get_direct_light(Direct_Light light, vec3 normal, vec3 view_direction) {
  vec3 light_direction = normalize(-light.direction);
  // Diffuse shading.
  float diffuse_factor = max(dot(normal, light_direction), 0.0);
  // Specular shading.
  vec3 reflection_direction = reflect(-light_direction, normal);
  float specular_factor = pow(max(dot(view_direction, reflection_direction),
                                  0.0), material.shininess);
  // Combine results.
  vec4 ambient, diffuse, specular;
  vec4 diffuse_map, specular_map;

  diffuse_map = vec4(texture(material.diffuse, texture_coordinates));
  specular_map = vec4(texture(material.specular, texture_coordinates));

  ambient = vec4(light.ambient,1.0f) * diffuse_map;
  diffuse = vec4(light.diffuse,1.0f) * diffuse_factor * diffuse_map;
  specular = vec4(light.specular,1.0f) * specular_factor * specular_map;
  return ambient + diffuse + specular;
}

vec4 get_point_light(Point_Light light, vec3 normal, vec3 fragment_position,
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

  vec4 diffuse_sampling = vec4(texture(material.diffuse, texture_coordinates));
  vec4 specular_sampling = vec4(texture(material.specular, texture_coordinates));

  vec4 ambient = vec4(light.ambient,1.0f) * diffuse_sampling;
  vec4 diffuse = vec4(light.diffuse,1.0f) * diffuse_factor * diffuse_sampling;
  vec4 specular = vec4(light.specular,1.0f) * specular_factor * specular_sampling;

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}
