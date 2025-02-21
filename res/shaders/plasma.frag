#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform float time;
uniform float speed;    // 1.0 = default animation speed
uniform float density;  // 1.0 = default wave frequency

void main() {
  vec2 uv = TexCoord;
  float t = time * speed;

  float v = 0.0;
  v += sin(uv.x * 10.0 * density + t);
  v += sin(uv.y * 10.0 * density + t * 0.7);
  v += sin((uv.x + uv.y) * 8.0 * density + t * 1.3);
  v += sin(length(uv - vec2(0.5 + 0.3 * sin(t), 0.5 + 0.3 * cos(t * 0.7))) * 12.0 * density);
  v = v * 0.25 + 0.5;

  float r = 0.5 + 0.5 * sin(v * 6.28318 + 0.000);
  float g = 0.5 + 0.5 * sin(v * 6.28318 + 2.094);
  float b = 0.5 + 0.5 * sin(v * 6.28318 + 4.189);

  FragColor = vec4(r, g, b, 1.0);
}
