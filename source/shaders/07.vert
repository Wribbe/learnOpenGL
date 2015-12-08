#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec3 Normal;
out vec3 Frag_position;

void main() {
  gl_Position = perspective * view * model * vec4(position, 1.0f);
  Normal = normal;
  Frag_position = vec3(model * vec4(position, 1.0f));
}
