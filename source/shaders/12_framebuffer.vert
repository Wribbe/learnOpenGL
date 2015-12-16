#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 v_texture_coordinates;

out vec2 texture_coordinates;

void main() {
  gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
  texture_coordinates = v_texture_coordinates;
}
