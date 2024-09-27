#pragma once

#include "graph.h"
#include "point.h"
#include "spring.h"
#include <atomic>
#include <cmath>
#include <ctime>
#include <map>

class ForceDirected {
public:
  ForceDirected(Graph *graph, float stiffness, float repulsion, float damping, float minEnergyThreshold = 0.01,
                float maxSpeed = INFINITY);
  Point *point(Node *node);

  Spring *spring(Edge *edge);

  // physics
  void applyCoulombsLaw();

  void applyHookesLaw();

  void attractToCentre();

  void updateVelocity(float timestep);

  void updatePosition(float timestep);

  void tick(float timestep);

  // Calculate the total kinetic energy of the system
  float totalEnergy();

  void start();

  std::atomic<bool> _started;
  std::atomic<bool> _stop;

  Graph *graph;
  float damping;
  float maxSpeed;
  float stiffness;
  float repulsion;
  float minEnergyThreshold;

  std::map<std::string, Point *> nodePoints;
  std::map<std::string, Spring *> edgeSprings;
};
