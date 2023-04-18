#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gui.h>
#include <math.h>
#include <stb/stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

struct MeshData {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLsizei indexCount;
  std::vector<GLuint> indices;
  std::vector<GLfloat> vertices;
  std::string filename;
  GLfloat x;
  GLfloat y;
  GLfloat z;
};

struct Player {
  float x;
  float y;
  float z;
  bool w;
  bool a;
  bool s;
  bool d;
};

Player player{0.0f, 0.0f, 5.0f, false, false, false, false};

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

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
    player.w = false;
  } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
    player.a = false;
  } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
    player.s = false;
  } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
    player.d = false;
  } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    player.w = true;
  } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    player.a = true;
  } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    player.s = true;
  } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    player.d = true;
  }
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

  char *vertex_shader_src = read_shader("res/vertex.shader");
  char *fragment_shader_src = read_shader("res/fragment.shader");

  GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_src);
  GLuint fragment_shader =
      create_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  std::vector<MeshData> meshDataList(2);
  meshDataList[0].filename = "blenderbox.obj";
  meshDataList[1].filename = "blenderbox.obj";
  meshDataList[0].x = 0.0f;
  meshDataList[0].y = 0.0f;
  meshDataList[0].z = 0.0f;
  meshDataList[1].x = 1.0f;
  meshDataList[1].y = 1.0f;
  meshDataList[1].z = 1.0f;

  for (unsigned int i = 0; i < 2; ++i) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        meshDataList[i].filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (scene && scene->HasMeshes()) {
      for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
          // Positions
          meshDataList[i].vertices.push_back(mesh->mVertices[j].x +
                                             meshDataList[i].x);
          meshDataList[i].vertices.push_back(mesh->mVertices[j].y +
                                             meshDataList[i].y);
          meshDataList[i].vertices.push_back(mesh->mVertices[j].z +
                                             meshDataList[i].z);

          // Texture coordinates
          if (mesh->mTextureCoords[0]) {
            meshDataList[i].vertices.push_back(mesh->mTextureCoords[0][j].x);
            meshDataList[i].vertices.push_back(mesh->mTextureCoords[0][j].y);
          } else {
            meshDataList[i].vertices.push_back(0.0f);
            meshDataList[i].vertices.push_back(0.0f);
          }
        }

        for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
          aiFace face = mesh->mFaces[k];
          for (unsigned int l = 0; l < face.mNumIndices; ++l) {
            meshDataList[i].indices.push_back(face.mIndices[l]);
          }
        }
      }
    } else {
      printf("Scene not loaded.\n");
      exit(EXIT_FAILURE);
    }

    glGenVertexArrays(1, &meshDataList[i].VAO);
    glGenBuffers(1, &meshDataList[i].VBO);
    glGenBuffers(1, &meshDataList[i].EBO);

    glBindVertexArray(meshDataList[i].VAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshDataList[i].VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 meshDataList[i].vertices.size() * sizeof(GLfloat),
                 &meshDataList[i].vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshDataList[i].EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshDataList[i].indices.size() * sizeof(GLuint),
                 &meshDataList[i].indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
  }

  GLuint texture;
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("texture.png", &width, &height, &nrChannels, 0);

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

  int screenWidth = 800;
  int screenHeight = 600;

  GLint ourTextureLocation =
      glGetUniformLocation(shader_program, "textureSampler");
  GLint time_location = glGetUniformLocation(shader_program, "time");
  GLint modelLoc = glGetUniformLocation(shader_program, "model");
  GLint viewLoc = glGetUniformLocation(shader_program, "view");
  GLint projLoc = glGetUniformLocation(shader_program, "projection");

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glfwSetKeyCallback(window, key_callback);

  while (!glfwWindowShouldClose(window)) {

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

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glUniform1i(ourTextureLocation, 0);

    glUniform1f(time_location, glfwGetTime());

    for (int i = 0; i < 2; i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model =
          glm::translate(model, glm::vec3(meshDataList[i].x, meshDataList[i].y,
                                          meshDataList[i].z));
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

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
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

      glBindVertexArray(meshDataList[i].VAO);
      glDrawElements(GL_TRIANGLES, meshDataList[i].indices.size(),
                     GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    imgui_render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // glDeleteVertexArrays(1, &VAO);
  // glDeleteBuffers(1, &VBO);
  // glDeleteProgram(shader_program);

  glfwTerminate();
}
