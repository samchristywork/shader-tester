#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform float time;
uniform float height;  // 1.0 = default flame height
uniform float speed;   // 1.0 = default rise speed

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
  for (int i = 0; i < 5; i++) {
    v += a * (noise(st) * 0.5 + 0.5);
    st *= 2.0;
    a *= 0.5;
  }
  return v;
}

void main() {
  vec2 uv = TexCoord;

  vec2 st = uv * vec2(2.0, 3.0);
  st.y -= time * 1.5 * speed;

  float n = fbm(st);
  float flame = n - uv.y * (1.0 / max(height, 0.01));
  flame = clamp(flame, 0.0, 1.0);

  vec3 col = vec3(0.0);
  col = mix(col, vec3(0.8, 0.0, 0.0), smoothstep(0.0, 0.3, flame));
  col = mix(col, vec3(1.0, 0.4, 0.0), smoothstep(0.2, 0.5, flame));
  col = mix(col, vec3(1.0, 0.8, 0.1), smoothstep(0.4, 0.7, flame));
  col = mix(col, vec3(1.0, 1.0, 0.9), smoothstep(0.6, 1.0, flame));

  FragColor = vec4(col, 1.0);
}
