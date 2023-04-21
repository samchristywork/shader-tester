#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>

struct Config {
  float x;
  float y;
  float z;
};

void imgui_render(Config *config);
void imgui_init(GLFWwindow *window);

#endif
