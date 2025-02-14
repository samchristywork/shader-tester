#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <controls.h>
#include <glm/gtc/type_ptr.hpp>
#include <gui.h>
#include <mesh.h>
#include <stb/stb_image.h>
#include <string>
#include <vector>

struct ShaderData {
  GLuint program;
  GLint time_loc;
  GLint model_loc;
  GLint view_loc;
  GLint proj_loc;
  GLint texture_loc;
  std::string name;
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
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, NULL, info_log);
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
    printf("GL ERROR: %s", message);
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
      printf("Unknown number of channels in texture\n");
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
    printf("Failed to load texture: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  stbi_image_free(data);

  return texture;
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

  free(vert_src);
  free(frag_src);

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);
  glDeleteShader(vert);
  glDeleteShader(frag);

  ShaderData sd;
  sd.program = prog;
  sd.name = name;
  sd.time_loc = glGetUniformLocation(prog, "time");
  sd.model_loc = glGetUniformLocation(prog, "model");
  sd.view_loc = glGetUniformLocation(prog, "view");
  sd.proj_loc = glGetUniformLocation(prog, "projection");
  sd.texture_loc = glGetUniformLocation(prog, "textureSampler");
  return sd;
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

  GLFWwindow *window = glfwCreateWindow(800, 600, "Shader Tester", NULL, NULL);

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
  config->shader_index = 0;
  config->polygon_mode = 0;
  config->render_mode = 0;

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

    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    update_player(window, player);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const ShaderData &shader = shaders[config->shader_index];
    glUseProgram(shader.program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[config->texture_index].texture);
    glUniform1i(shader.texture_loc, 0);
    glUniform1f(shader.time_loc, (float)glfwGetTime());

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

    imgui_render(config, mesh_names, texture_names, shader_names);

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
