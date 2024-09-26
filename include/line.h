#pragma once

#include "camera.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/ext.hpp>
#include <vector>

class Line {
  int shaderProgram;
  unsigned int VBO, VAO;
  std::vector<float> vertices;
  glm::vec3 startPoint;
  glm::vec3 endPoint;
  glm::mat4 MVP = glm::mat4(1.0);
  glm::vec3 lineColor;

public:
  int setMVP(glm::mat4 mvp);
  int setColor(glm::vec3 color);
  void setVertices(glm::vec3 start, glm::vec3 end);
  int draw(Camera *);
  void init();
  Line(glm::vec3 start, glm::vec3 end);
  ~Line();
};
