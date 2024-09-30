#pragma once

#include "vec.h"
#include <string>

struct Color {
  float r, g, b;
};

struct NodeData {
  Color color;
};

class Node {
public:
  Node(std::string id /*, data*/) : p(Vec::random()), v({0, 0, 0}), a({0, 0, 0}) {
    this->id = id;
    // this.data = (data != = undefined) ? data : {};

    // Data fields used by layout algorithm in this file:
    // this.data.mass
    // Data used by default renderer in springyui.js
    // this.data.label
    m = 1.0; // mass
    // p = Vec::random(); // position
    // v = Vec(0, 0, 0);  // velocity
    // a = Vec(0, 0, 0);  // acceleration
  };

  Vec p;
  Vec v;
  Vec a;
  float m;
  std::string id;
  NodeData data = {.color = {1.0f, 1.0f, 1.0f}};

  void applyForce(Vec force) { a = a.add(force.divide(m)); }
};
