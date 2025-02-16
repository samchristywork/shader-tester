#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float time;
uniform float noise_scale; // 1.0 = original (10x)
uniform float speed;       // 1.0 = original speed
uniform vec3 tint;         // (1,1,1) = no tint

vec2 random2(vec2 st) {
  st = vec2(dot(st, vec2(127.1, 311.7)), dot(st, vec2(269.5, 183.3)));
  return -1.0 + 2.0 * fract(sin(st) * 43758.5453123);
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

float perlinNoise(vec2 st, float persistence, int octaves) {
  float value = 0.0;
  float amplitude = 1.0;
  float frequency = 1.0;
  float maxValue = 0.0;

  for (int i = 0; i < octaves; ++i) {
    value += amplitude * noise(st * frequency);
    maxValue += amplitude;
    amplitude *= persistence;
    frequency *= 2.0;
  }

  return value / maxValue;
}

void main() {
  vec2 st = TexCoord;
  st.x += time * 0.1 * speed;
  float noiseValue = perlinNoise(st * 10.0 * noise_scale, 0.5, 4) + 0.5;
  vec4 noiseColor = vec4(vec3(noiseValue) * tint, 1.0);

  FragColor = texture(textureSampler, TexCoord);
  FragColor = mix(FragColor, noiseColor, 0.5);
}
