#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
uniform float time;
uniform vec3 holo_color;  // scan/glow tint color
uniform float scan_speed; // 1.0 = default scan line speed
uniform float flicker;    // 1.0 = default flicker intensity

void main() {
  // Scan lines
  float scan = sin(TexCoord.y * 180.0 - time * 5.0 * scan_speed) * 0.5 + 0.5;
  scan = pow(scan, 6.0) * 0.4;

  // Sample texture luminance as a mask
  vec4 tex = texture(textureSampler, TexCoord);
  float lum = dot(tex.rgb, vec3(0.299, 0.587, 0.114));

  // Random flicker
  float f = 1.0 - flicker * 0.12 * abs(sin(time * 37.0) * sin(time * 13.0));

  // Vignette
  vec2 uv = TexCoord - 0.5;
  float vignette = 1.0 - dot(uv, uv) * 2.0;
  vignette = max(vignette, 0.0);

  vec3 col = holo_color * (lum + scan * 2.0) * vignette * f;
  float alpha = (lum * 0.8 + scan) * vignette * f;
  FragColor = vec4(col, alpha);
}
