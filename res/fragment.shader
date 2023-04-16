#version 330 core

out vec4 FragColor;
uniform float colorValue;

void main() {
  FragColor = vec4(0.0, colorValue, 0.0, 1.0);
}
