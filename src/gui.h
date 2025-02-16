#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

enum class UniformType { Float, Vec3, Vec4, Bool, Int };

struct UniformValue {
  std::string name;
  int location;
  UniformType type;
  float data[4]; // float: data[0]; vec3: data[0..2]; vec4: data[0..3]
  int i_val;     // bool (0/1) and int
};

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
                  const std::vector<std::string> &shader_names,
                  std::vector<UniformValue> &custom_uniforms);
void imgui_init(GLFWwindow *window);

#endif
