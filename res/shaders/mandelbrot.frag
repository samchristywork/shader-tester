#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform float time;
uniform float zoom_speed; // 1.0 = original speed
uniform bool animate;     // true = zoom over time, false = static

void main() {
  float effective_time = animate ? time : 0.0;
  float zoom = pow(0.5, effective_time * 0.3 * zoom_speed);
  vec2 center = vec2(-0.7269, 0.1889);

  vec2 c = (TexCoord - vec2(0.5)) * vec2(3.5, 2.5) * zoom + center;

  vec2 z = vec2(0.0);
  int max_iter = 256;
  int i;
  for (i = 0; i < max_iter; i++) {
    if (dot(z, z) > 4.0) break;
    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
  }

  if (i == max_iter) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    // Smooth iteration count for anti-banding
    float t = float(i) - log2(log2(dot(z, z))) + 4.0;
    t = t / float(max_iter);

    // Color cycling
    float r = 0.5 + 0.5 * cos(6.28318 * (t + 0.0));
    float g = 0.5 + 0.5 * cos(6.28318 * (t + 0.33));
    float b = 0.5 + 0.5 * cos(6.28318 * (t + 0.67));
    FragColor = vec4(r, g, b, 1.0);
  }
}
