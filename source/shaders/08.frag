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

  float ambient_strength = 0.1f;
  vec3 ambient = ambient_strength * light_color;

  vec3 norm = normalize(Normal);
  vec3 light_direction = normalize(light_position - Frag_position);
  float diff = max(dot(norm, light_direction), 0.0f);
  vec3 diffuse = diff * light_color;

  vec3 view_direction = normalize(view_position - Frag_position);
  vec3 reflect_direction = reflect(-light_direction, norm);
  float specular_strength = 0.5f;
  float specular_value = pow(max(dot(view_direction, reflect_direction), 0.0f), 32);
  vec3 specular = specular_strength * specular_value * light_color;

  vec3 result = (ambient + diffuse + specular) * object_color;
  color = vec4(result, 1.0f);
}
