#version 330 core
in vec3 our_color;
in vec2 texture_coordinates_vertex;

uniform sampler2D our_texture_01;

out vec4 color;

void main() {
  color = texture(our_texture_01, texture_coordinates_vertex);
}
