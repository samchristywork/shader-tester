#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float threshold;  // 0.0 = fully visible, 1.0 = fully dissolved
uniform vec3 edge_color;  // burn edge tint

vec2 random2(vec2 p) {
  p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
  return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);
  vec2 u = f * f * (3.0 - 2.0 * f);
  return mix(mix(dot(random2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
                 dot(random2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
             mix(dot(random2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
                 dot(random2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x),
             u.y);
}

float fbm(vec2 st) {
  float v = 0.0;
  float a = 0.5;
  for (int i = 0; i < 4; i++) {
    v += a * (noise(st) * 0.5 + 0.5);
    st *= 2.0;
    a *= 0.5;
  }
  return v;
}

void main() {
  float n = fbm(TexCoord * 4.0);

  if (n < threshold) discard;

  vec4 tex = texture(textureSampler, TexCoord);
  float edge = smoothstep(threshold, threshold + 0.08, n);
  vec3 col = mix(edge_color, tex.rgb, edge);
  FragColor = vec4(col, tex.a);
}
