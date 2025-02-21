#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float pixel_size; // 1.0 = 32 blocks across, larger = more pixelated

void main() {
  float blocks = max(32.0 * pixel_size, 1.0);
  vec2 uv = floor(TexCoord * blocks) / blocks + vec2(0.5) / blocks;
  FragColor = texture(textureSampler, uv);
}
