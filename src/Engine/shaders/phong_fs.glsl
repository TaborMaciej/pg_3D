#version 460

layout(location=0) out vec4 vFragColor;

layout(std140, binding=0) uniform Modifiers {
    vec4  Kd;
    bool use_map_Kd;
};

in vec2 vertex_texcoords;
in vec3 vertex_normals_in_vs;
in vec3 vertex_coords_in_vs;

uniform sampler2D map_Kd;

void main() {
    if (use_map_Kd)
        vFragColor = Kd * texture(map_Kd, vertex_texcoords);
    else
        vFragColor = Kd;

    vec3 normal = normalize(vertex_normals_in_vs);
    //vFragColor.rgb = normal;
    vFragColor.rgb = abs(vertex_coords_in_vs);
}