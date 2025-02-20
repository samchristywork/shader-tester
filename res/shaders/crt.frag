#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float curvature;  // 1.0 = default barrel distortion
uniform float scan_dark;  // 1.0 = default scanline darkness

void main() {
  // Barrel distortion
  vec2 uv = TexCoord - vec2(0.5);
  float r2 = dot(uv, uv);
  uv *= 1.0 + curvature * 0.3 * r2;
  uv += vec2(0.5);

  if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  vec4 col = texture(textureSampler, uv);

  // Scanlines
  float scan = sin(uv.y * 600.0) * 0.5 + 0.5;
  scan = pow(scan, 2.0);
  col.rgb *= 1.0 - scan_dark * 0.3 * (1.0 - scan);

  // Vignette
  float vig = 1.0 - dot((TexCoord - 0.5) * 1.5, (TexCoord - 0.5) * 1.5);
  col.rgb *= clamp(vig, 0.0, 1.0);

  // Phosphor tint
  col.g *= 1.05;

  FragColor = col;
}
