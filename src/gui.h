#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

struct Config {
  int mesh_index;
  int texture_index;
  int shader_index;
  int polygon_mode;
  int render_mode; // 0 = model, 1 = quad
  bool reload_requested;
  bool screenshot_requested;
  std::string shader_error;
};

void imgui_render(Config *config,
                  const std::vector<std::string> &mesh_names,
                  const std::vector<std::string> &texture_names,
                  const std::vector<std::string> &shader_names);
void imgui_init(GLFWwindow *window);

#endif
