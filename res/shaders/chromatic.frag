#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float time;
uniform float aberration; // 1.0 = default offset amount
uniform bool animate;     // true = pulse over time

void main() {
  vec2 center = vec2(0.5);
  vec2 dir = normalize(TexCoord - center);
  float dist = length(TexCoord - center);

  float effective_time = animate ? time : 0.0;
  float offset = 0.015 * aberration * (1.0 + 0.5 * sin(effective_time * 2.0));

  float r = texture(textureSampler, TexCoord + dir * offset * dist).r;
  float g = texture(textureSampler, TexCoord).g;
  float b = texture(textureSampler, TexCoord - dir * offset * dist).b;

  FragColor = vec4(r, g, b, 1.0);
}
