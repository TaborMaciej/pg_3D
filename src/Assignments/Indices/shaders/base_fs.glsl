#version 410

in vec4 gl_Color;
layout(location=0) out vec4 vFragColor;

void main() {
    vFragColor = gl_Color;
}
