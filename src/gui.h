#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>

struct Config {
  float x;
  float y;
  float z;
  int polygon_mode;
};

void imgui_render(Config *config);
void imgui_init(GLFWwindow *window);

#endif
