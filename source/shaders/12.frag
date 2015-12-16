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

  /* Trying to make anisotropic work */

  vec4 sampled_specular = vec4(texture(material.specular, texture_coordinates));

  vec3 halfway = normalize(light_direction - view_direction);
  float dot_normal_light = clamp(dot(normal, light_direction), 0.0f, 1.0f);
  float dot_halfway_aniso = dot(normalize(normal + sampled_specular.rgb),
                                halfway);
  float aniso_offset = 0.0f;
  float aniso = max(0, sin(radians((dot_halfway_aniso + aniso_offset) * 180)));

  float specular_factor = clamp(dot(normal, halfway), 0.0f, 1.0f);
  float lerped = specular_factor*(1-sampled_specular.a) + aniso*sampled_specular.a;
  float gloss = 0.5f;
  specular_factor = clamp(pow(lerped, gloss * 128) * 0.5f, 0.0f, 1.0f);

  vec3 try_color;
  vec3 albedo = vec3(0.0f, 1.0f, 0.0f);
  vec3 light_color = vec3(1.0f);
  //try_color.rgb = vec3((albedo * light_color * dot_normal_light) +
  //                     light_color * specular);
  //return vec4(try_color, 1.0f);

  //vec3 sampled_specular = vec3(texture(material.specular, texture_coordinates));
  //float dot_light_direction_normal = dot(light_direction, normal);

  ////vec3 reflect_direction = reflect(-light_direction, sampled_specular);
  //vec3 reflect_direction = reflect(-light_direction, normal);

  //float alpha_x = 0.3f;
  //float alpha_y = 0.3f;

  //vec3 halfway_vector = normalize(light_direction + view_direction);
  //vec3 binormal_direction = cross(normal, sampled_specular);

  //float specular_factor;
  //if (dot_light_direction_normal < 0.0) {
  //  specular_factor = 0.0f;
  //} else {
  //  float dot_halfway_normal = dot(halfway_vector, normal);
  //  float dot_view_normal = dot(view_direction, normal);
  //  float dot_halfway_tangent = dot(halfway_vector, sampled_specular);
  //  float dot_halfway_binormal = dot(halfway_vector, binormal_direction);
  //  float dot_halfway_tangent_alpha_x = dot_halfway_tangent / alpha_x;
  //  float dot_halfway_tangent_alpha_y = dot_halfway_binormal / alpha_y;

  //  specular_factor = sqrt(max(0.0,
  //                            dot_light_direction_normal / dot_view_normal))
  //                            * exp(-2.0f *
  //                                    (dot_halfway_tangent_alpha_x *
  //                                    dot_halfway_tangent_alpha_y) /
  //                            (1.0f + dot_halfway_normal));
  //  //specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0),
  //  //                      material.shininess);
  //  //specular_factor *= 0.01f;
  //  specular_factor = sampled_specular.
  //}


  /* */

  //float diffuse_factor = max(dot_light_direction_normal, 0.0);
  float diffuse_factor = max(dot_normal_light, 0.0);
  // Specular shading.
  //vec3 reflect_direction = reflect(-light_direction, normal);
  //float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0),
  //                            material.shininess);
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
  //vec4 specular_sampling = vec4(texture(material.specular, texture_coordinates));

  vec4 ambient = vec4(light.ambient,1.0f) * diffuse_sampling;
  vec4 diffuse = vec4(light.diffuse,1.0f) * diffuse_factor * diffuse_sampling;
  //vec4 specular = vec4(light.specular,1.0f) * specular_factor * specular_sampling;
  vec4 specular = vec4(light.specular,1.0f) * specular_factor;

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}
