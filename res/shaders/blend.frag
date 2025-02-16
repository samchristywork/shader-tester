#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureSampler;
uniform sampler2D textureSampler2;
uniform float mix_factor;

void main() {
  vec4 tex1 = texture(textureSampler, TexCoord);
  vec4 tex2 = texture(textureSampler2, TexCoord);
  FragColor = mix(tex1, tex2, mix_factor);
}
