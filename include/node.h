#pragma once

#include <string>

struct Color {
  float r, g, b;
};

struct NodeData {
  Color color;
};

class Node {
public:
  Node(std::string id /*, data*/) {
    this->id = id;
    // this.data = (data != = undefined) ? data : {};

    // Data fields used by layout algorithm in this file:
    // this.data.mass
    // Data used by default renderer in springyui.js
    // this.data.label
  };

  std::string id;
  NodeData data = {.color = {1.0f, 1.0f, 1.0f}};
};
