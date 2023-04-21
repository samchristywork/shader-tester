#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D ourTexture;
uniform float time;

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
  st.x += time * 0.1;
  float noiseValue = perlinNoise(st * 10.0, 0.5, 4) + 0.5;
  vec4 noise = vec4(vec3(noiseValue), 1.0);

  FragColor = texture(ourTexture, TexCoord);
  FragColor = mix(FragColor, noise, 0.5);
}
