#version 330 core

uniform mat4 model;
uniform mat4 perspective;
uniform mat4 view;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coord;

out vec2 vertex_texture_coords;

void main() {
  vertex_texture_coords = texture_coord;
  gl_Position = perspective * view * model * vec4 (position, 1.0f);
}
