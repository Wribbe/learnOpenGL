#version 330 core

in vec3 our_color;
in vec3 vertex_position;

out vec4 color;

void main() {
  //color = vec4(our_color, 1.0f);
  color = vec4(vertex_position, 1.0f);
}
