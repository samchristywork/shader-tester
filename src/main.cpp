#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <GL/glew.h>
#include <controls.h>
#include <glm/gtc/type_ptr.hpp>
#include <gui.h>
#include <mesh.h>
#include <sstream>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <string>
#include <vector>

struct ShaderData {
  GLuint program;
  GLint time_loc;
  GLint model_loc;
  GLint view_loc;
  GLint proj_loc;
  GLint texture_loc;
  GLint texture2_loc;
  std::string name;
  std::string vert_path;
  std::string frag_path;
  std::vector<UniformValue> custom_uniforms;
};

struct TextureData {
  GLuint texture;
  std::string name;
};

Player player{0.0f, 5.0f, 0.0f, 0.0f, 10.0f};

void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

GLuint create_shader(GLenum type, const char *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, nullptr, info_log);
    fprintf(stderr, "Error: Shader compilation failed: %s\n", info_log);
    exit(EXIT_FAILURE);
  }

  return shader;
}

char *read_shader(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "Error: Could not open shader file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  rewind(fp);

  char *buffer = (char *)malloc(size + 1);
  fread(buffer, 1, size, fp);
  buffer[size] = '\0';

  fclose(fp);

  return buffer;
}

static void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *message,
                                     const void *userParam) {
  if (type == GL_DEBUG_TYPE_ERROR) {
    fprintf(stderr, "GL ERROR: %s", message);
  }
}

void enableOpenGLDebugging() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(glDebugOutput, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_TRUE);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLuint load_texture(const char *filename) {
  GLuint texture;
  int width, height, nrChannels;
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

  if (data) {
    GLint internalFormat = 0;
    GLint format = 0;
    switch (nrChannels) {
    case 1:
      internalFormat = GL_R8;
      format = GL_RED;
      break;
    case 2:
      internalFormat = GL_RG8;
      format = GL_RG;
      break;
    case 3:
      internalFormat = GL_RGB8;
      format = GL_RGB;
      break;
    case 4:
      internalFormat = GL_RGBA8;
      format = GL_RGBA;
      break;
    default:
      fprintf(stderr, "Unknown number of channels in texture\n");
      exit(EXIT_FAILURE);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fprintf(stderr, "Failed to load texture: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  stbi_image_free(data);

  return texture;
}

static const char *builtin_uniforms[] = {
    "time", "model", "view", "projection", "textureSampler", "textureSampler2", nullptr};

static bool is_builtin_uniform(const std::string &name) {
  for (int i = 0; builtin_uniforms[i]; i++)
    if (name == builtin_uniforms[i])
      return true;
  return false;
}

// Parse a single shader source for custom uniforms (float/vec3/vec4/bool/int).
static void parse_uniforms_from_source(const char *src, GLuint prog,
                                       std::vector<UniformValue> &out) {
  std::istringstream stream(src);
  std::string line;
  while (std::getline(stream, line)) {
    size_t u = line.find("uniform");
    if (u == std::string::npos)
      continue;
    // Skip if "uniform" is preceded by a comment
    size_t comment = line.find("//");
    if (comment != std::string::npos && comment < u)
      continue;

    std::istringstream ls(line.substr(u + 7));
    std::string type_str, name_str;
    ls >> type_str >> name_str;
    // Strip trailing semicolon and array brackets
    size_t end = name_str.find_first_of(";[");
    if (end != std::string::npos)
      name_str = name_str.substr(0, end);
    if (name_str.empty() || is_builtin_uniform(name_str))
      continue;

    UniformType type;
    if (type_str == "float")
      type = UniformType::Float;
    else if (type_str == "vec3")
      type = UniformType::Vec3;
    else if (type_str == "vec4")
      type = UniformType::Vec4;
    else if (type_str == "bool")
      type = UniformType::Bool;
    else if (type_str == "int")
      type = UniformType::Int;
    else
      continue;

    int loc = glGetUniformLocation(prog, name_str.c_str());
    if (loc == -1)
      continue;

    // Deduplicate (same uniform may appear in both vert and frag)
    bool dup = false;
    for (const auto &existing : out)
      if (existing.name == name_str) {
        dup = true;
        break;
      }
    if (dup)
      continue;

    UniformValue uv;
    uv.name = name_str;
    uv.location = loc;
    uv.type = type;
    uv.data[0] = uv.data[1] = uv.data[2] = uv.data[3] = 1.0f;
    uv.i_val = (type == UniformType::Bool) ? 1 : 0;
    out.push_back(uv);
  }
}

static std::vector<UniformValue> parse_custom_uniforms(const char *vert_src,
                                                       const char *frag_src,
                                                       GLuint prog) {
  std::vector<UniformValue> result;
  parse_uniforms_from_source(vert_src, prog, result);
  parse_uniforms_from_source(frag_src, prog, result);
  return result;
}

static std::string basename_no_ext(const std::string &path) {
  size_t slash = path.find_last_of("/\\");
  size_t start = (slash == std::string::npos) ? 0 : slash + 1;
  size_t dot = path.find_last_of('.');
  size_t end = (dot == std::string::npos || dot <= start) ? path.size() : dot;
  return path.substr(start, end - start);
}

ShaderData load_shader_program(const std::string &name, const char *vert_path,
                               const char *frag_path) {
  char *vert_src = read_shader(vert_path);
  char *frag_src = read_shader(frag_path);

  GLuint vert = create_shader(GL_VERTEX_SHADER, vert_src);
  GLuint frag = create_shader(GL_FRAGMENT_SHADER, frag_src);

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);

  GLint link_success;
  glGetProgramiv(prog, GL_LINK_STATUS, &link_success);
  if (!link_success) {
    char info_log[512];
    glGetProgramInfoLog(prog, 512, nullptr, info_log);
    fprintf(stderr, "Error: Shader linking failed: %s\n", info_log);
    exit(EXIT_FAILURE);
  }

  glDeleteShader(vert);
  glDeleteShader(frag);

  ShaderData sd;
  sd.program = prog;
  sd.name = name;
  sd.vert_path = vert_path;
  sd.frag_path = frag_path;
  sd.time_loc = glGetUniformLocation(prog, "time");
  sd.model_loc = glGetUniformLocation(prog, "model");
  sd.view_loc = glGetUniformLocation(prog, "view");
  sd.proj_loc = glGetUniformLocation(prog, "projection");
  sd.texture_loc = glGetUniformLocation(prog, "textureSampler");
  sd.texture2_loc = glGetUniformLocation(prog, "textureSampler2");
  sd.custom_uniforms = parse_custom_uniforms(vert_src, frag_src, prog);
  free(vert_src);
  free(frag_src);
  return sd;
}

// Returns 0 on failure (prints error but does not exit). Appends error to error_out.
static GLuint try_create_shader(GLenum type, const char *src, std::string &error_out) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, nullptr, info_log);
    fprintf(stderr, "Error: Shader compilation failed: %s\n", info_log);
    error_out += info_log;
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

// Reloads shader from disk. Keeps the existing program on failure.
// Sets shader_error on failure, clears it on success.
static void reload_shader(ShaderData &sd, std::string &shader_error) {
  FILE *vf = fopen(sd.vert_path.c_str(), "r");
  FILE *ff = fopen(sd.frag_path.c_str(), "r");
  if (!vf || !ff) {
    shader_error = "Could not open shader files for reload";
    fprintf(stderr, "Error: %s\n", shader_error.c_str());
    if (vf) fclose(vf);
    if (ff) fclose(ff);
    return;
  }
  fclose(vf);
  fclose(ff);

  char *vert_src = read_shader(sd.vert_path.c_str());
  char *frag_src = read_shader(sd.frag_path.c_str());

  std::string error;
  GLuint vert = try_create_shader(GL_VERTEX_SHADER, vert_src, error);
  GLuint frag = try_create_shader(GL_FRAGMENT_SHADER, frag_src, error);

  if (!vert || !frag) {
    free(vert_src);
    free(frag_src);
    shader_error = error;
    if (vert) glDeleteShader(vert);
    if (frag) glDeleteShader(frag);
    return;
  }

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);
  glDeleteShader(vert);
  glDeleteShader(frag);

  GLint link_success;
  glGetProgramiv(prog, GL_LINK_STATUS, &link_success);
  if (!link_success) {
    free(vert_src);
    free(frag_src);
    char info_log[512];
    glGetProgramInfoLog(prog, 512, nullptr, info_log);
    fprintf(stderr, "Error: Shader linking failed: %s\n", info_log);
    shader_error = info_log;
    glDeleteProgram(prog);
    return;
  }

  auto new_uniforms = parse_custom_uniforms(vert_src, frag_src, prog);
  free(vert_src);
  free(frag_src);
  // Preserve existing values for uniforms that survived the reload
  for (auto &nu : new_uniforms) {
    for (const auto &old : sd.custom_uniforms) {
      if (old.name == nu.name && old.type == nu.type) {
        for (int k = 0; k < 4; k++) nu.data[k] = old.data[k];
        nu.i_val = old.i_val;
        break;
      }
    }
  }

  glDeleteProgram(sd.program);
  sd.program = prog;
  sd.time_loc = glGetUniformLocation(prog, "time");
  sd.model_loc = glGetUniformLocation(prog, "model");
  sd.view_loc = glGetUniformLocation(prog, "view");
  sd.proj_loc = glGetUniformLocation(prog, "projection");
  sd.texture_loc = glGetUniformLocation(prog, "textureSampler");
  sd.texture2_loc = glGetUniformLocation(prog, "textureSampler2");
  sd.custom_uniforms = std::move(new_uniforms);
  shader_error.clear();
  fprintf(stderr, "Shader '%s' reloaded successfully\n", sd.name.c_str());
}

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Error: GLFW initialization failed\n");
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback(glfw_error_callback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Shader Tester", nullptr, nullptr);

  glfwSetWindowSize(window, 800, 600);
  glfwSetWindowPos(window, 100, 100);

  if (!window) {
    fprintf(stderr, "Error: GLFW window creation failed\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Error: GLEW initialization failed\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  enableOpenGLDebugging();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  imgui_init(window);

  std::vector<ShaderData> shaders;
  shaders.push_back(load_shader_program(
      "flat", "res/shaders/flat.vert", "res/shaders/flat.frag"));
  shaders.push_back(load_shader_program(
      "perlin", "res/shaders/perlin.vert", "res/shaders/perlin.frag"));
  shaders.push_back(load_shader_program(
      "ripple", "res/shaders/ripple.vert", "res/shaders/ripple.frag"));
  shaders.push_back(load_shader_program(
      "mandelbrot", "res/shaders/mandelbrot.vert", "res/shaders/mandelbrot.frag"));
  shaders.push_back(load_shader_program(
      "blend", "res/shaders/blend.vert", "res/shaders/blend.frag"));

  std::vector<std::string> shader_names;
  for (const auto &s : shaders) shader_names.push_back(s.name);

  std::vector<MeshData> meshDataList;
  load_model(&meshDataList, "res/models/box.obj");
  load_model(&meshDataList, "res/models/circle.obj");
  load_model(&meshDataList, "res/models/cone.obj");
  load_model(&meshDataList, "res/models/cylinder.obj");
  load_model(&meshDataList, "res/models/grid.obj");
  load_model(&meshDataList, "res/models/icosphere.obj");
  load_model(&meshDataList, "res/models/monkey.obj");
  load_model(&meshDataList, "res/models/plane.obj");
  load_model(&meshDataList, "res/models/torus.obj");
  load_model(&meshDataList, "res/models/uvsphere.obj");

  std::vector<std::string> mesh_names;
  for (const auto &m : meshDataList) mesh_names.push_back(basename_no_ext(m.filename));

  std::vector<TextureData> textures;
  textures.push_back({load_texture("res/textures/gradient.png"), "gradient"});
  textures.push_back(
      {load_texture("res/textures/checkerboard.png"), "checkerboard"});
  textures.push_back({load_texture("res/textures/rand.png"), "rand"});

  std::vector<std::string> texture_names;
  for (const auto &t : textures) texture_names.push_back(t.name);

  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_cursor_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Fullscreen quad (NDC coords, identity matrices will fill the screen)
  float quad_verts[] = {
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
  };
  unsigned int quad_indices[] = {0, 1, 2, 0, 2, 3};
  GLuint quadVAO, quadVBO, quadEBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glGenBuffers(1, &quadEBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  Config *config = (Config *)malloc(sizeof(Config));
  config->mesh_index = 0;
  config->texture_index = 0;
  config->texture2_index = 1;
  config->shader_index = 0;
  config->polygon_mode = 0;
  config->render_mode = 0;
  config->reload_requested = false;
  config->screenshot_requested = false;

  int screenWidth = 0;
  int screenHeight = 0;
  while (!glfwWindowShouldClose(window)) {
    switch (config->polygon_mode) {
    case 0:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    case 1:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case 2:
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      break;
    }

    if (config->reload_requested) {
      reload_shader(shaders[config->shader_index], config->shader_error);
      config->reload_requested = false;
    }

    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    update_player(window, player);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderData &shader = shaders[config->shader_index];
    glUseProgram(shader.program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[config->texture_index].texture);
    glUniform1i(shader.texture_loc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[config->texture2_index].texture);
    if (shader.texture2_loc != -1)
      glUniform1i(shader.texture2_loc, 1);

    glUniform1f(shader.time_loc, (float)glfwGetTime());

    for (const auto &uv : shader.custom_uniforms) {
      switch (uv.type) {
      case UniformType::Float:
        glUniform1f(uv.location, uv.data[0]);
        break;
      case UniformType::Vec3:
        glUniform3fv(uv.location, 1, uv.data);
        break;
      case UniformType::Vec4:
        glUniform4fv(uv.location, 1, uv.data);
        break;
      case UniformType::Bool:
      case UniformType::Int:
        glUniform1i(uv.location, uv.i_val);
        break;
      }
    }

    glm::mat4 identity = glm::mat4(1.0f);

    if (config->render_mode == 1) {
      // Fullscreen quad: pass identity matrices so NDC coords pass through
      glUniformMatrix4fv(shader.model_loc, 1, GL_FALSE, glm::value_ptr(identity));
      glUniformMatrix4fv(shader.view_loc, 1, GL_FALSE, glm::value_ptr(identity));
      glUniformMatrix4fv(shader.proj_loc, 1, GL_FALSE, glm::value_ptr(identity));
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glBindVertexArray(quadVAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    } else {
      glUniformMatrix4fv(shader.model_loc, 1, GL_FALSE, glm::value_ptr(identity));

      glBindVertexArray(meshDataList[config->mesh_index].VAO);
      glDrawElements(GL_TRIANGLES,
                     (GLsizei)meshDataList[config->mesh_index].indices.size(),
                     GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      glm::vec3 cameraPos = glm::vec3(player.x, player.y, player.z);
      glm::mat4 view =
          glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

      float aspectRatio = (float)screenWidth / (float)screenHeight;
      glm::mat4 projection =
          glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

      glUniformMatrix4fv(shader.view_loc, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(shader.proj_loc, 1, GL_FALSE,
                         glm::value_ptr(projection));
    }

    imgui_render(config, mesh_names, texture_names, shader_names,
                 shader.custom_uniforms);

    if (config->screenshot_requested) {
      config->screenshot_requested = false;
      int w, h;
      glfwGetFramebufferSize(window, &w, &h);
      std::vector<unsigned char> pixels(w * h * 3);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
      // Flip vertically: OpenGL origin is bottom-left, PNG expects top-left
      for (int row = 0; row < h / 2; ++row) {
        std::swap_ranges(pixels.begin() + row * w * 3,
                         pixels.begin() + (row + 1) * w * 3,
                         pixels.begin() + (h - 1 - row) * w * 3);
      }
      if (stbi_write_png("screenshot.png", w, h, 3, pixels.data(), w * 3)) {
        fprintf(stderr, "Screenshot saved to screenshot.png\n");
      } else {
        fprintf(stderr, "Error: Failed to save screenshot\n");
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (auto &meshData : meshDataList) {
    glDeleteVertexArrays(1, &meshData.VAO);
    glDeleteBuffers(1, &meshData.VBO);
    glDeleteBuffers(1, &meshData.EBO);
  }
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &quadEBO);
  for (auto &s : shaders) glDeleteProgram(s.program);
  free(config);

  glfwTerminate();
}
