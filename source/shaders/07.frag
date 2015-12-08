#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 Frag_position;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;

void main() {

  vec3 norm = normalize(Normal);
  vec3 light_direction = normalize((light_position/4.0) - Frag_position);

  float diff = max(dot(norm, light_direction), 0.0f);
  vec3 diffuse = diff * light_color;

  float ambient_strength = 0.1f;
  vec3 ambient = (diffuse + ambient_strength) * light_color;

  vec3 result = ambient * object_color;
  color = vec4(result, 1.0f);
}
