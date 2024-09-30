#pragma once

#include "node.h"
#include <string>

class Edge {
public:
  Edge(std::string id, Node *source, Node *target /*, data */);

  float k;
  float length;
  Node *source;
  Node *target;
  std::string id;
};
