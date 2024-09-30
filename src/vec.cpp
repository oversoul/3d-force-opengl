#include "vec.h"
#include <cstdlib>
#include <random>

Vec::Vec(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
Vec::Vec(float p) : x(p), y(p), z(p) {}

Vec Vec::random() {
  srand(time(NULL));
  float lower_bound = 0;
  float upper_bound = 10;
  std::random_device rd;
  std::minstd_rand gen(rd());
  std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
  float rand_x = unif(rd);
  float rand_y = unif(rd);
  float rand_z = unif(rd);
  return {rand_x, rand_y, rand_z}; // random
}

Vec Vec::add(float n) const { return {x + n, y + n, z + n}; }
Vec Vec::add(const Vec &v2) const { return {x + v2.x, y + v2.y, z + v2.z}; }
Vec Vec::subtract(const Vec &v2) const { return {x - v2.x, y - v2.y, z - v2.z}; };
// inline Vec Vec::multiply(float n) { return {x * n, y * n, z * n}; }

// Avoid divide by zero errors..
Vec Vec::divide(float n) const { return (n != 0) ? Vec{x / n, y / n, z / n} : Vec{0, 0, 0}; }
float Vec::magnitudeSquared() const { return x * x + y * y + z * z; }
float Vec::magnitude() const { return std::sqrt(magnitudeSquared()); }
Vec Vec::normalise() const { return divide(magnitude()); }
