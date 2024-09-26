#include "line.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/ext.hpp>
#include <iostream>

using namespace std;

// GL_LINES is deprecated, use this instead
// https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
Line::Line(glm::vec3 start, glm::vec3 end) {
  startPoint = start;
  endPoint = end;
  lineColor = glm::vec3(0.5, 0.5, 0.5);
}

void Line::init() {
  const char *vertexShaderSource = R""""(
    #version 300 es
    layout (location = 0) in vec3 aPos;
    uniform mat4 MVP;
    void main() {
        gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    }
    )"""";

  const char *fragmentShaderSource = R""""(
    #version 300 es
    precision mediump float;
    out vec4 FragColor;
    uniform vec3 color;
    void main() {
        FragColor = vec4(color, 1.0f);
    })"""";

  int success;
  char infoLog[512];
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  // check for shader compile errors
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  };
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  // check for shader compile errors
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  };

  // link shaders
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // vertices = {
  //     start.x,
  //     start.y,
  //     start.z,
  //     end.x,
  //     end.y,
  //     end.z,

  // };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void Line::setVertices(glm::vec3 start, glm::vec3 end) {
  vertices = {
      start.x, start.y, start.z, end.x, end.y, end.z,
  };
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
}

int Line::setMVP(glm::mat4 mvp) {
  MVP = mvp;
  return 1;
}

int Line::setColor(glm::vec3 color) {
  lineColor = color;
  return 1;
}

int Line::draw(Camera *) {
  glUseProgram(shaderProgram);

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
  glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

  glBindVertexArray(VAO);
  glDrawArrays(GL_LINES, 0, 2);
  return 1;
}

Line::~Line() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
}
