#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <mesh.h>

void load_model(std::vector<MeshData> *meshDataList, const char *filename) {
  MeshData *meshData = new MeshData;

  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (scene && scene->HasMeshes()) {
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[i];
      for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
        // Positions
        meshData->vertices.push_back(mesh->mVertices[j].x);
        meshData->vertices.push_back(mesh->mVertices[j].y);
        meshData->vertices.push_back(mesh->mVertices[j].z);

        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
          meshData->vertices.push_back(mesh->mTextureCoords[0][j].x);
          meshData->vertices.push_back(mesh->mTextureCoords[0][j].y);
        } else {
          meshData->vertices.push_back(0.0f);
          meshData->vertices.push_back(0.0f);
        }
      }

      for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
        aiFace face = mesh->mFaces[k];
        for (unsigned int l = 0; l < face.mNumIndices; ++l) {
          meshData->indices.push_back(face.mIndices[l]);
        }
      }
    }
  } else {
    printf("Scene not loaded.\n");
    exit(EXIT_FAILURE);
  }

  glGenVertexArrays(1, &meshData->VAO);
  glGenBuffers(1, &meshData->VBO);
  glGenBuffers(1, &meshData->EBO);

  glBindVertexArray(meshData->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, meshData->VBO);
  glBufferData(GL_ARRAY_BUFFER, meshData->vertices.size() * sizeof(GLfloat),
               &meshData->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               meshData->indices.size() * sizeof(GLuint), &meshData->indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (GLvoid *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  meshDataList->push_back(*meshData);
}
