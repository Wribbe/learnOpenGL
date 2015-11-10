#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture_coordinates;

out vec3 our_color;
out vec2 texture_coordinates_vertex;

uniform mat4 transform;

void main()
{
  gl_Position = transform * vec4(position, 1.0f);
  our_color = color;
  texture_coordinates_vertex = vec2(texture_coordinates.x,
                                    1.0f - texture_coordinates.y);
}
