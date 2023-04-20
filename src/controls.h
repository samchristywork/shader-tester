#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>

struct Player {
  float x;
  float y;
  float z;
  bool w;
  bool a;
  bool s;
  bool d;
  bool space;
  bool shift;
};

void update_player(GLFWwindow *window, struct Player &player);

#endif
