#pragma once

#include "node.h"
#include <string>

class Edge {
public:
  Edge(std::string id, Node *source, Node *target /*, data */);

  float length;
  Node *source;
  Node *target;
  float coefficient;
  std::string id;
};
