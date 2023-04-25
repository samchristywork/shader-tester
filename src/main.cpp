#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <controls.h>
#include <glm/gtc/type_ptr.hpp>
#include <gui.h>
#include <stb/stb_image.h>

struct ObjectData {
  int mesh_index;
  GLfloat x;
  GLfloat y;
  GLfloat z;
};

Player player{0.0f, 0.0f, 0.0f, 0.0f, 5.0f};

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

ObjectData load_object(int idx, float x, float y, float z) {
  return ObjectData {
    idx,
    x,
    y,
    z
  };
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

  GLFWwindow *window = glfwCreateWindow(800, 600, "Hello, World!", NULL, NULL);

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

  char *vertex_shader_src = read_shader("res/shaders/vertex.shader");
  char *fragment_shader_src = read_shader("res/shaders/fragment.shader");

  GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_src);
  GLuint fragment_shader =
      create_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

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

  std::vector<ObjectData> objects;
  objects.push_back(load_object(0, -6, 0, 0));
  objects.push_back(load_object(2, -3, 0, 0));
  objects.push_back(load_object(5, 0, 0, 0));
  objects.push_back(load_object(6, 3, 0, 0));
  objects.push_back(load_object(7, 6, 0, 0));

  GLuint texture;
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("res/textures/gradient.png", &width, &height, &nrChannels, 0);

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
    printf("Failed to load texture\n");
  }

  stbi_image_free(data);

  glGenerateMipmap(GL_TEXTURE_2D);

  float angle = 0.0f;

  GLint ourTextureLocation =
      glGetUniformLocation(shader_program, "textureSampler");
  GLint time_location = glGetUniformLocation(shader_program, "time");
  GLint modelLoc = glGetUniformLocation(shader_program, "model");
  GLint viewLoc = glGetUniformLocation(shader_program, "view");
  GLint projLoc = glGetUniformLocation(shader_program, "projection");

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glfwSetKeyCallback(window, key_callback);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Config *config = (Config *)malloc(sizeof(Config));
  config->x = 0;
  config->y = 0;
  config->z = 0;
  config->polygon_mode = 0;

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

    glUseProgram(shader_program);

    glUniform1i(ourTextureLocation, 0);

    glUniform1f(time_location, glfwGetTime());

    for (int i = 0; i < objects.size(); i++) {
      glm::mat4 model = glm::mat4(1.0f);

      model = glm::translate(
          model, glm::vec3(objects[i].x, objects[i].y, objects[i].z));

      if(i==0){
        model = glm::translate(
            model, glm::vec3(config->x, config->y, config->z));
      }

      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      glBindVertexArray(meshDataList[objects[i].mesh_index].VAO);
      glDrawElements(GL_TRIANGLES,
                     meshDataList[objects[i].mesh_index].indices.size(),
                     GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }

    // View matrix
    glm::vec3 cameraPos = glm::vec3(player.x, player.y, player.z);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    // Projection matrix
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    float fov = glm::radians(45.0f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 projection =
        glm::perspective(fov, aspectRatio, nearPlane, farPlane);

    // Pass the matrices to the shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    imgui_render(config);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (auto &meshData : meshDataList) {
    glDeleteVertexArrays(1, &meshData.VAO);
    glDeleteBuffers(1, &meshData.VBO);
    glDeleteBuffers(1, &meshData.EBO);
  }

  glfwTerminate();
}
