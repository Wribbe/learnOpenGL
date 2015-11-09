#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 vertex_texture_coordinates;

out vec3 our_color;
out vec2 texture_coordinates;

void main() {
  gl_Position = vec4(position, 1.0f);
  our_color = color;
  texture_coordinates = vertex_texture_coordinates;
}
