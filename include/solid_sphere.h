#pragma once

#include "camera.h"
#include "shader.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

class SolidSphere {

protected:
  float radius;

  std::vector<GLfloat> vertices;
  std::vector<GLfloat> normals;
  std::vector<GLfloat> texcoords;
  std::vector<GLushort> indices;
  int shaderProgram;
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
  unsigned int VBO, VAO, instanceVBO;
  glm::vec3 color = glm::vec3(1, 1, 1);

  std::vector<glm::vec3> offsets;
  unsigned int _numberOfOffsetsToDraw = 0;

public:
  SolidSphere(float radius, unsigned int rings, unsigned int sectors);
  void draw(Camera *);
  void init();
  void setMVP(glm::mat4x4 m, glm::mat4x4 v, glm::mat4x4 p);
  void setColor(glm::vec3 c);
  Shader *shader;
  void generateOffsets();

  void debugOffsets();
  void addOffset(const glm::vec3 &newOffset);
};
