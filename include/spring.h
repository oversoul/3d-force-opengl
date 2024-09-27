#pragma once

#include "point.h"

class Spring {
public:
  Spring(Point *point1, Point *point2, float length, float k) {
    this->point1 = point1;
    this->point2 = point2;
    this->length = length; // spring length at rest
    this->k = k;           // spring constant (See Hooke's law) .. how stiff the spring is
  }

  float k;
  float length;
  Point *point1;
  Point *point2;
};
