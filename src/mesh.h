#ifndef MESH_H
#define MESH_H

#include <GL/gl.h>
#include <string>
#include <vector>

struct MeshData {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  std::vector<GLuint> indices;
  std::vector<GLfloat> vertices;
  std::string filename;
};

void load_model(std::vector<MeshData> *meshDataList, const char *filename);

#endif
