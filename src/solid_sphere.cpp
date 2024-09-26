#include "solid_sphere.h"
#include "shader.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>

// Utility class for drawing a sphere
// Adapted to modern OpenGL from
// https://stackoverflow.com/questions/5988686/creating-a-3d-sphere-in-opengl-using-visual-c
SolidSphere::SolidSphere(float r, unsigned int rings, unsigned int sectors) : radius(r) {}

void SolidSphere::init() {

  shader = new Shader("shaders/basic.vs", "shaders/basic.fs");

  //   const char *vertexShaderSource = R""""(
  // #version 300 es
  // layout (location = 0) in vec3 aPos;
  // uniform mat4 model;
  // uniform mat4 view;
  // uniform mat4 projection;
  // void main() {
  //     gl_Position = projection * view * model * vec4(aPos, 1.0f);
  // }
  // )"""";
  //
  //   const char *fragmentShaderSource = R""""(
  // #version 300 es
  // precision mediump float;
  // out vec4 FragColor;
  // uniform vec3 color;
  // void main() {
  //     FragColor = vec4(color, 1.0f);
  // }
  // )"""";
  // shader.

  GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles
  // GLfloat radius = 1.0f;
  int gradation = 20;
  vertices.clear();
  for (alpha = 0.0; alpha < M_PI; alpha += M_PI / gradation) {
    for (beta = 0.0; beta < 2.01 * M_PI; beta += M_PI / gradation) {
      x = radius * cos(beta) * sin(alpha);
      y = radius * sin(beta) * sin(alpha);
      z = radius * cos(alpha);
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

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0,        // attribute. No particular reason for 1, but must match the layout in the shader.
                        3,        // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );

  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void SolidSphere::setMVP(glm::mat4 m, glm::mat4 v, glm::mat4 p) {
  view = v;
  model = m;
  projection = p;
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

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);
}
