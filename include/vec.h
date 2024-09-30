#pragma once

#include <ctime>

class Vec {
public:
  float x;
  float y;
  float z;

  Vec(float _x, float _y, float _z);
  Vec(float p);
  static Vec random();

  Vec add(float n) const;
  Vec add(const Vec &v2) const;
  Vec subtract(const Vec &v2) const;
  // Vec multiply(float n);
  inline Vec multiply(float n) const { return {x * n, y * n, z * n}; }

  Vec operator*(float r) const { return Vec(x * r, y * r, z * r); }
  Vec operator+(const Vec &r) const { return Vec(x + r.x, y + r.y, z + r.z); }
  Vec operator-(const Vec &r) const { return Vec(x - r.x, y - r.y, z - r.z); }

  // Avoid divide by zero errors..
  Vec normalise() const;
  float magnitude() const;
  float magnitudeSquared() const;
  Vec divide(float n) const;
};
