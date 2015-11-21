#version 330 core

uniform sampler2D texture_sampler;

in vec2 vertex_texture_coords;

out vec4 color;

void main() {
  color = texture(texture_sampler, vertex_texture_coords);
}
