#include <controls.h>
#include <math.h>

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

  player.x=player.r*sin(player.theta);
  player.z=player.r*cos(player.theta);
}
