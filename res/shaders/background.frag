#version 330 core
in vec2 vNDC;
out vec4 FragColor;
uniform float time;
uniform mat4 inv_view_proj;
uniform vec3 camera_pos;

void main() {
    // Reconstruct world-space ray direction for this fragment
    vec4 world = inv_view_proj * vec4(vNDC, -1.0, 1.0);
    vec3 ray_dir = normalize(world.xyz / world.w - camera_pos);

    // h > 0 = above horizon (sky), h < 0 = below (ground)
    float h = ray_dir.y;

    float t = time * 0.04;
    vec3 sky_top   = vec3(0.05, 0.08, 0.28) + 0.03 * vec3(sin(t), 0.0, cos(t * 0.7));
    vec3 sky_mid   = vec3(0.18, 0.28, 0.48);
    vec3 horizon_c = vec3(0.62, 0.50, 0.32) + 0.05 * vec3(sin(t * 0.5), cos(t * 0.3), 0.0);
    vec3 ground_c  = vec3(0.10, 0.09, 0.07);

    vec3 sky = mix(sky_mid, sky_top, smoothstep(0.0, 0.6, h));
    vec3 color = mix(ground_c, sky, smoothstep(-0.04, 0.04, h));
    color = mix(color, horizon_c, exp(-abs(h) * 18.0) * 0.75);

    FragColor = vec4(color, 1.0);
}
