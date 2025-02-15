#include <controls.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <math.h>

extern Player player;

void update_player(GLFWwindow *window, struct Player &player) {
  if (glfwGetKey(window, GLFW_KEY_W)) {
    player.r -= 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    player.r += 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    player.theta -= 0.02f;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    player.theta += 0.02f;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    player.y += 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
    player.y -= 0.1f;
  }

  player.x = player.r * sin(player.theta);
  player.z = player.r * cos(player.theta);
}

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  static double last_x = 0.0;
  static double last_y = 0.0;
  static bool first = true;

  if (first) {
    last_x = xpos;
    last_y = ypos;
    first = false;
    return;
  }

  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
      !ImGui::GetIO().WantCaptureMouse) {
    double dx = xpos - last_x;
    double dy = ypos - last_y;
    player.theta += (float)(dx * 0.005);
    player.y -= (float)(dy * 0.02);
  }

  last_x = xpos;
  last_y = ypos;
}
