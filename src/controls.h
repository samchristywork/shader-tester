#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>

struct Player {
  float x;
  float y;
  float z;
  float theta;
  float r;
};

void update_player(GLFWwindow *window, struct Player &player);

#endif
