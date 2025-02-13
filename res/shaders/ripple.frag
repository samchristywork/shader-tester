#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float time;

void main() {
  vec2 st = TexCoord - vec2(0.5);
  float dist = length(st);
  vec2 dir = dist > 0.0 ? normalize(st) : vec2(0.0);
  vec2 displaced = TexCoord + dir * sin(dist * 25.0 - time * 4.0) * 0.04;
  FragColor = texture(textureSampler, displaced);
}
