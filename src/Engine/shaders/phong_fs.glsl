#version 460

layout(location=0) out vec4 vFragColor;

layout(std140, binding=0) uniform Material {
    vec4 Ka;
    vec4 Kd;
    vec4 Ks;
    vec4 Ns_use_map;
};

const int MAX_POINT_LIGHTS=24;

struct PointLight {
    vec4 position_in_view_space;
    vec4 color;
    vec4 intensity_radius;
};

layout(std140, binding=2) uniform Lights {
    vec4 ambient;
    uvec4 n_p_lights;
    PointLight p_light[MAX_POINT_LIGHTS];
};

in vec2 vertex_texcoords;
in vec3 vertex_normals_in_vs;
in vec3 vertex_coords_in_vs;

uniform sampler2D map_Kd;

void main() {
    vec3 baseColor = Kd.rgb;
    if (Ns_use_map.y > 0.5)
        baseColor *= texture(map_Kd, vertex_texcoords).rgb;

    vec3 normal = normalize(vertex_normals_in_vs);
    vec3 view_dir = normalize(-vertex_coords_in_vs);
    float Ns = Ns_use_map.x;
    vec3 Ks_color = Ks.rgb;

    uint n = n_p_lights.x;
    vec3 color = Ka.rgb * ambient.xyz;

    for (uint i = 0u; i < n; ++i) {
        vec3 lightPos = p_light[i].position_in_view_space.xyz;
        vec3 L = normalize(lightPos - vertex_coords_in_vs);
        float diff = max(dot(normal, L), 0.0);
        float intensity = p_light[i].intensity_radius.x;
        vec3 light_color = p_light[i].color.xyz * intensity;

        vec3 H = normalize(L + view_dir);
        float spec = 0.0;
        float NdotH = max(dot(normal, H), 0.0);
        if (NdotH > 0.0) {
            spec = pow(NdotH, Ns);
        }

        color += baseColor * light_color * diff;
        color += Ks_color * light_color * spec;
    }

    vFragColor = vec4(color, Kd.a);
}