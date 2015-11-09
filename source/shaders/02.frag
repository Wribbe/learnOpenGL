#version 330 core
in vec3 our_color;
in vec2 texture_coordinates;

out vec4 color;

uniform sampler2D our_texture;

void main() {
  color = vec4(our_color, 1.0f) * texture(our_texture, texture_coordinates);
}
