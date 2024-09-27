#pragma once

#include <ctime>

class Vec {
public:
  float x;
  float y;
  float z;

  Vec(float _x, float _y, float _z);
  static Vec *random();

  Vec *add(Vec *v2);
  Vec *subtract(Vec *v2);
  Vec *multiply(float n);

  // Avoid divide by zero errors..
  Vec *normalise();
  float magnitude();
  Vec *divide(float n);
};
