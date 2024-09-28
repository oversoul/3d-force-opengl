#include "vec.h"
#include <cstdlib>
#include <random>

Vec::Vec(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

Vec *Vec::random() {
  srand(time(NULL));
  double lower_bound = -10;
  double upper_bound = 10;
  std::random_device rd;
  std::minstd_rand gen(rd());
  std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
  double rand_x = unif(rd);
  double rand_y = unif(rd);
  double rand_z = unif(rd);
  return new Vec(rand_x, rand_y, rand_z); // random
}

Vec *Vec::add(Vec *v2) { return new Vec(x + v2->x, y + v2->y, z + v2->z); }
Vec *Vec::subtract(Vec *v2) { return new Vec(x - v2->x, y - v2->y, z - v2->z); };
Vec *Vec::multiply(float n) { return new Vec(x * n, y * n, z * n); }

// Avoid divide by zero errors..
Vec *Vec::divide(float n) {
  auto vx = (n != 0) ? x / n : 0;
  auto vy = (n != 0) ? y / n : 0;
  auto vz = (n != 0) ? z / n : 0;

  return new Vec(vx, vy, vz);
}

float Vec::magnitude() { return std::sqrt(x * x + y * y + z * z); }
Vec *Vec::normalise() { return divide(magnitude()); }
