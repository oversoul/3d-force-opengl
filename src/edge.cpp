#include "edge.h"

Edge::Edge(std::string id, Node *source, Node *target /*, data */) {
  this->id = id;
  this->source = source;
  this->target = target;
  // this->data = (data != = undefined) ? data : {};

  k = 200.0;
  length = 1.0f;
  // Edge data field used by layout alorithm
  // this.data.length
  // this.data.type
}
