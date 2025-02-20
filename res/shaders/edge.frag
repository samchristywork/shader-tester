#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float edge_strength; // 1.0 = default magnitude
uniform float threshold;     // 0.0 = show all edges
uniform vec3 edge_color;     // color drawn on detected edges

void main() {
  vec2 tx = vec2(1.0 / 512.0);

  float tl = dot(texture(textureSampler, TexCoord + tx * vec2(-1, -1)).rgb, vec3(0.299, 0.587, 0.114));
  float tm = dot(texture(textureSampler, TexCoord + tx * vec2( 0, -1)).rgb, vec3(0.299, 0.587, 0.114));
  float tr = dot(texture(textureSampler, TexCoord + tx * vec2( 1, -1)).rgb, vec3(0.299, 0.587, 0.114));
  float ml = dot(texture(textureSampler, TexCoord + tx * vec2(-1,  0)).rgb, vec3(0.299, 0.587, 0.114));
  float mr = dot(texture(textureSampler, TexCoord + tx * vec2( 1,  0)).rgb, vec3(0.299, 0.587, 0.114));
  float bl = dot(texture(textureSampler, TexCoord + tx * vec2(-1,  1)).rgb, vec3(0.299, 0.587, 0.114));
  float bm = dot(texture(textureSampler, TexCoord + tx * vec2( 0,  1)).rgb, vec3(0.299, 0.587, 0.114));
  float br = dot(texture(textureSampler, TexCoord + tx * vec2( 1,  1)).rgb, vec3(0.299, 0.587, 0.114));

  float gx = -tl - 2.0 * ml - bl + tr + 2.0 * mr + br;
  float gy = -tl - 2.0 * tm - tr + bl + 2.0 * bm + br;
  float edge = sqrt(gx * gx + gy * gy) * edge_strength;

  vec4 original = texture(textureSampler, TexCoord);
  float t = smoothstep(threshold, threshold + 0.1, edge);
  FragColor = vec4(mix(original.rgb, edge_color, t), 1.0);
}
