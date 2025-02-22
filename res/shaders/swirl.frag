#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float time;
uniform float strength; // 1.0 = default twist amount
uniform bool animate;   // true = spin over time

void main() {
  vec2 uv = TexCoord - vec2(0.5);
  float dist = length(uv);
  float angle = atan(uv.y, uv.x);

  float effective_time = animate ? time : 0.0;
  float twist = strength * 5.0 * smoothstep(0.5, 0.0, dist) + effective_time * 0.5;

  float new_angle = angle + twist;
  vec2 rotated = vec2(cos(new_angle), sin(new_angle)) * dist + vec2(0.5);

  FragColor = texture(textureSampler, rotated);
}
