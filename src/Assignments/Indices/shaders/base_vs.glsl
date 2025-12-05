#version 410

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec4 a_color;

out vec4 gl_Color;

void main() {
    gl_Position = a_vertex_position;
    gl_Color = a_color;
}
