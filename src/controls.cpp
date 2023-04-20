#include <controls.h>

void update_player(GLFWwindow *window, struct Player &player) {
  if (player.w) {
    player.z -= 0.1f;
  }
  if (player.s) {
    player.z += 0.1f;
  }
  if (player.a) {
    player.x -= 0.1f;
  }
  if (player.d) {
    player.x += 0.1f;
  }
  if (player.space) {
    player.y += 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
    player.y -= 0.1f;
  }
}
