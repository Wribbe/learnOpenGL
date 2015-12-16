#version 330 core
in vec2 texture_coordinates;

out vec4 color;

uniform sampler2D screen_texture;

void main() {
  color = texture(screen_texture, texture_coordinates);
  //color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
