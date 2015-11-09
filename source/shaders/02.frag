#version 330 core
in vec3 our_color;
in vec2 texture_coordinates;

out vec4 color;

uniform sampler2D our_texture_01;
uniform sampler2D our_texture_02;

void main() {
  color = mix(texture(our_texture_01, texture_coordinates),
              texture(our_texture_02, vec2(texture_coordinates.x,
                                           1.0f - texture_coordinates.y)),
              0.2);
}
