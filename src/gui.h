#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>

void imgui_render();
struct Config {
  float x;
  float y;
  float z;
};

void imgui_init(GLFWwindow *window);

#endif
