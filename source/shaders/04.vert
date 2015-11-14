#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 vertex_texture_coordinates;

out vec2 texture_coordinates;

void main() {
  texture_coordinates = vertex_texture_coordinates;
  gl_Position = vec4(position, 1.0f);
}
