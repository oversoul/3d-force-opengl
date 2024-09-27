#pragma once

#include "vec.h"

class Point {
public:
  Point(Vec *position, float mass) {
    p = position;         // position
    m = mass;             // mass
    v = new Vec(0, 0, 0); // velocity
    a = new Vec(0, 0, 0); // acceleration
  }

  void applyForce(Vec *force) { a = a->add(force->divide(m)); }

  Vec *p;
  Vec *v;
  Vec *a;
  float m;
};
