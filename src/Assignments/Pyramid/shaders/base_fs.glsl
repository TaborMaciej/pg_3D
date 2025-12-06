#version 420

layout(std140, binding = 0) uniform Modifier {
    float strength;
    vec3  color; 
};

in vec4 fs_color;
layout(location=0) out vec4 vFragColor;

void main() {
    vec3 modified = fs_color.rgb * strength * color;
    vFragColor = vec4(modified, 1);
}
