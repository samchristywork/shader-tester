#include <controls.h>
#include <math.h>

void update_player(GLFWwindow *window, struct Player &player) {
  if (glfwGetKey(window, GLFW_KEY_W)) {
    player.z -= 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    player.z += 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    player.x -= 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    player.x += 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    player.y += 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
    player.y -= 0.1f;
  }
}
