#pragma once

#include "graph.h"
#include "octree.h"
#include <cmath>
#include <ctime>

class ForceDirected {
public:
  ForceDirected(Graph *graph, float stiffness, float repulsion, float damping, float minEnergyThreshold = 0.01,
                float maxSpeed = INFINITY);

  // physics
  void applyCoulombsLaw();
  void applyHookesLaw();
  void attractToCentre();

  void updateVelocity(float timestep);
  void updatePosition(float timestep);
  void applySubtleMovement(float timestep);

  void tick(float timestep);

  // Calculate the total kinetic energy of the system
  float totalEnergy();

  void start();

  bool stopped = false;

  Graph *graph;
  float damping;
  float maxSpeed;
  float stiffness;
  float repulsion;
  float minEnergyThreshold;

  float time;
  float movementAmplitude;
  float movementFrequency;

  BarnesHut *barnesHut;

  std::vector<Vec> originalPositions;
};
