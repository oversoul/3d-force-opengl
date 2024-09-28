#include "solid_sphere.h"
#include "shader.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/ext.hpp>
#include <iostream>
#include <ostream>
#include <vector>

// Utility class for drawing a sphere
// Adapted to modern OpenGL from
// https://stackoverflow.com/questions/5988686/creating-a-3d-sphere-in-opengl-using-visual-c
SolidSphere::SolidSphere(float r, unsigned int rings, unsigned int sectors) : radius(r) {}

void SolidSphere::init() {

  shader = new Shader("shaders/basic.vs", "shaders/basic.fs");

  vertices.clear();

  int gradation = 2;
  for (GLfloat alpha = 0.0; alpha < M_PI; alpha += M_PI / gradation) {
    for (GLfloat beta = 0.0; beta < 2.01 * M_PI; beta += M_PI / gradation) {
      auto x = radius * cos(beta) * sin(alpha);
      auto y = radius * sin(beta) * sin(alpha);
      auto z = radius * cos(alpha);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
      x = radius * cos(beta) * sin(alpha + M_PI / gradation);
      y = radius * sin(beta) * sin(alpha + M_PI / gradation);
      z = radius * cos(alpha + M_PI / gradation);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    }
  }

  // generateOffsets();

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &instanceVBO);

  glBindVertexArray(VAO);

  // Setup vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
  glEnableVertexAttribArray(0);

  // Setup instance buffer
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), offsets.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribDivisor(1, 1); // Tell OpenGL this is an instanced vertex attribute.

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  debugOffsets();
}

void SolidSphere::debugOffsets() {
  // Debug: Print the first few offsets
  std::cout << "Number of offsets: " << offsets.size() << std::endl;
  for (int i = 0; i < std::min(5, static_cast<int>(offsets.size())); ++i) {
    std::cout << "Offset " << i << ": " << offsets[i].x << ", " << offsets[i].y << ", " << offsets[i].z << std::endl;
  }
}

void SolidSphere::generateOffsets() {
  offsets.clear();
  // const int gridSize = 1;
  // const float spacing = 3.0f;
  // for (int x = -gridSize / 2; x <= gridSize / 2; ++x) {
  //   for (int y = -gridSize / 2; y <= gridSize / 2; ++y) {
  //     for (int z = -gridSize / 2; z <= gridSize / 2; ++z) {
  //       offsets.push_back(glm::vec3(x * spacing, y * spacing, z * spacing));
  //     }
  //   }
  // }
}

void SolidSphere::setMVP(glm::mat4 m, glm::mat4 v, glm::mat4 p) {
  view = v;
  model = m;
  projection = p;
}

void SolidSphere::addOffset(const glm::vec3 &newOffset) {
  offsets.push_back(newOffset);

  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), offsets.data(), GL_STATIC_DRAW);
}

void SolidSphere::setColor(glm::vec3 c) { color = c; }

void SolidSphere::draw(Camera *camera) {
  shader->use();

  shader->setVec3("viewPos", camera->pos);
  shader->setVec3("lightPos", camera->pos);
  shader->setVec3("lightColor", {1.0f, 1.0f, 1.0f});

  shader->setMat4("model", model);
  shader->setMat4("view", view);
  shader->setMat4("projection", projection);
  shader->setVec3("color", color);

  // glBindVertexArray(VAO);
  // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

  // glDrawElementsInstanced(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_SHORT, 0, _numberOfOffsetsToDraw);
  glBindVertexArray(VAO);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertices.size() / 3, offsets.size());
  // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

  // Debug output
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cout << "OpenGL error: " << error << std::endl;
  }
}
