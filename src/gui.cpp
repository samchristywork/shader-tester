#include <gui.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>

void imgui_init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void imgui_render(Config *config,
                  const std::vector<std::string> &mesh_names,
                  const std::vector<std::string> &texture_names,
                  const std::vector<std::string> &shader_names,
                  std::vector<UniformValue> &custom_uniforms) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Controls");

  std::vector<const char *> mesh_items, texture_items, shader_items;
  for (const auto &s : mesh_names) mesh_items.push_back(s.c_str());
  for (const auto &s : texture_names) texture_items.push_back(s.c_str());
  for (const auto &s : shader_names) shader_items.push_back(s.c_str());

  const char *render_mode_items[] = {"Model", "Quad"};
  ImGui::ListBox("Render Mode", &config->render_mode, render_mode_items, 2);

  if (config->render_mode == 0) {
    ImGui::ListBox("Model", &config->mesh_index, mesh_items.data(),
                   (int)mesh_items.size());
  }
  ImGui::ListBox("Texture", &config->texture_index, texture_items.data(),
                 (int)texture_items.size());
  ImGui::ListBox("Shader", &config->shader_index, shader_items.data(),
                 (int)shader_items.size());

  const char *poly_items[] = {"Fill", "Line", "Point"};
  if (config->render_mode == 0) {
    ImGui::ListBox("Polygon Mode", &config->polygon_mode, poly_items, 3);
  }

  if (ImGui::Button("Reload Shader")) {
    config->reload_requested = true;
  }

  if (ImGui::Button("Save Screenshot")) {
    config->screenshot_requested = true;
  }

  if (!custom_uniforms.empty()) {
    ImGui::Separator();
    ImGui::Text("Uniforms");
    for (auto &uv : custom_uniforms) {
      switch (uv.type) {
      case UniformType::Float:
        ImGui::DragFloat(uv.name.c_str(), &uv.data[0], 0.01f);
        break;
      case UniformType::Vec3:
        ImGui::ColorEdit3(uv.name.c_str(), uv.data);
        break;
      case UniformType::Vec4:
        ImGui::ColorEdit4(uv.name.c_str(), uv.data);
        break;
      case UniformType::Bool: {
        bool b = (uv.i_val != 0);
        if (ImGui::Checkbox(uv.name.c_str(), &b))
          uv.i_val = b ? 1 : 0;
        break;
      }
      case UniformType::Int:
        ImGui::DragInt(uv.name.c_str(), &uv.i_val);
        break;
      }
    }
  }

  if (!config->shader_error.empty()) {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Shader error:");
    ImGui::TextWrapped("%s", config->shader_error.c_str());
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
