#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 Frag_position;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform Material material;

void main() {

  vec3 ambient = light_color * material.ambient;

  vec3 norm = normalize(Normal);
  vec3 light_direction = normalize(light_position - Frag_position);
  float diff = max(dot(norm, light_direction), 0.0f);
  vec3 diffuse = light_color * (diff * material.diffuse);

  vec3 view_direction = normalize(view_position - Frag_position);
  vec3 reflect_direction = reflect(-light_direction, norm);
  float specular_value = pow(max(dot(view_direction, reflect_direction), 0.0f),
                                     material.shininess);
  vec3 specular = (specular_value * material.specular) * light_color;

  vec3 result = ambient + diffuse + specular;
  color = vec4(result, 1.0f);
}
