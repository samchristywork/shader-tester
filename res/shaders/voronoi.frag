#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform float time;
uniform float scale;   // 1.0 = default cell density
uniform vec3 color_a;  // cell interior color
uniform vec3 color_b;  // cell far-edge color

vec2 hash2(vec2 p) {
  p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
  return fract(sin(p) * 43758.5453);
}

void main() {
  vec2 st = TexCoord * 6.0 * scale;
  vec2 i_st = floor(st);
  vec2 f_st = fract(st);

  float min_dist = 10.0;
  float second_dist = 10.0;

  for (int y = -1; y <= 1; y++) {
    for (int x = -1; x <= 1; x++) {
      vec2 neighbor = vec2(float(x), float(y));
      vec2 point = hash2(i_st + neighbor);
      point = 0.5 + 0.5 * sin(time * 0.5 + 6.2831 * point);
      vec2 diff = neighbor + point - f_st;
      float d = length(diff);
      if (d < min_dist) {
        second_dist = min_dist;
        min_dist = d;
      } else if (d < second_dist) {
        second_dist = d;
      }
    }
  }

  float edge = smoothstep(0.0, 0.05, second_dist - min_dist);
  vec3 col = mix(color_a, color_b, min_dist);
  col = mix(vec3(0.0), col, edge);
  FragColor = vec4(col, 1.0);
}
