#include <gui.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void imgui_init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void imgui_render() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Window");
  ImGui::Text("Hello, World!");

  Config *config = (Config *)malloc(sizeof(Config));
  ImGui::SliderFloat("x", &config->x, -1.0f, 1.0f);
  ImGui::SliderFloat("y", &config->y, -1.0f, 1.0f);
  ImGui::SliderFloat("z", &config->z, -1.0f, 1.0f);
  free(config);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
