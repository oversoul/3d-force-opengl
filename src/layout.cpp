#include "layout.h"

ForceDirected::ForceDirected( //
    Graph *graph,             //
    float stiffness,          //
    float repulsion,          //
    float damping,            //
    float minEnergyThreshold, //
    float maxSpeed            //
    )
    : time(0), movementAmplitude(0.1f), movementFrequency(0.5f) {

  this->graph = graph;
  this->damping = damping;                       // velocity damping factor
  this->maxSpeed = maxSpeed;                     // nodes aren't allowed to exceed this speed
  this->repulsion = repulsion;                   // repulsion constant
  this->stiffness = stiffness;                   // spring stiffness constant
  this->minEnergyThreshold = minEnergyThreshold; // threshold used to determine render stop
  barnesHut = new BarnesHut(0.5f);
}

// physics
void ForceDirected::applyCoulombsLaw() {

  barnesHut->buildTree(graph->nodes);

  for (auto &node : graph->nodes) {
    barnesHut->calculateForces(node, repulsion);
  }

  // double cutoff_radius = 100.0;                                 // Choose an appropriate value for your scale
  // double cutoff_radius_squared = cutoff_radius * cutoff_radius; // Pre-compute for efficiency
  //
  // for (auto point1 : graph->nodes) {
  //   for (auto point2 : graph->nodes) {
  //     if (point1 != point2) {
  //       auto d = point1->p.subtract(point2->p);
  //
  //       double distance_squared = d.x * d.x + d.y * d.y + d.z * d.z;
  //
  //       if (distance_squared < cutoff_radius_squared) {
  //         // Calculate and apply forces as normal
  //         auto distance = d.magnitude() + 0.1; // avoid massive forces at small distances (and divide by zero)
  //         auto direction = d.normalise();
  //
  //         // apply force to each end point
  //         point1->applyForce(direction.multiply(repulsion).divide(distance * distance * 0.5));
  //         point2->applyForce(direction.multiply(repulsion).divide(distance * distance * -0.5));
  //       }
  //     }
  //   }
  // }
}

void ForceDirected::applyHookesLaw() {
  for (auto e : graph->edges) {
    // auto s = spring1(e);
    auto d = e->target->p.subtract(e->source->p); // the direction of the spring
    auto displacement = e->length - d.magnitude();
    auto direction = d.normalise();

    // apply force to each end point
    e->source->applyForce(direction.multiply(e->k * displacement * -0.5));
    e->target->applyForce(direction.multiply(e->k * displacement * 0.5));
  }

  // for (const auto &edge : graph->edges) {
  //   Node *source = edge->source;
  //   Node *target = edge->target;
  //
  //   Vec direction = target->p.subtract(source->p);
  //   float distance = direction.magnitude();
  //
  //   // Avoid division by zero
  //   if (distance > 0) {
  //     // Calculate the spring force
  //     float force = stiffness * (distance - edge->length);
  //     Vec springForce = direction.normalise().multiply(force);
  //
  //     // Apply force to both nodes
  //     source->applyForce(springForce);
  //     target->applyForce(springForce.multiply(-1));
  //   }
  // }
}

void ForceDirected::attractToCentre() {
  for (auto pt : graph->nodes) {
    auto direction = pt->p.multiply(-1.0);
    pt->applyForce(direction.multiply(repulsion / 50.0));
  }
}

void ForceDirected::updateVelocity(float timestep) {
  for (auto pt : graph->nodes) {
    // Is this, along with updatePosition below, the only places that your
    // integration code exist?
    pt->v = pt->v.add(pt->a.multiply(timestep)).multiply(damping);
    if (pt->v.magnitude() > maxSpeed) {
      pt->v = pt->v.normalise().multiply(maxSpeed);
    }
    pt->a = Vec(0, 0, 0);
  }
}

void ForceDirected::updatePosition(float timestep) {
  for (auto pt : graph->nodes) {
    // Same question as above; along with updateVelocity, is this all of
    // your integration code?
    pt->p = pt->p.add(pt->v.multiply(timestep));

    // TODO: mix
    // Is this, along with updatePosition below, the only places that your
    // integration code exist?
    pt->v = pt->v.add(pt->a.multiply(timestep)).multiply(damping);
    if (pt->v.magnitude() > maxSpeed) {
      pt->v = pt->v.normalise().multiply(maxSpeed);
    }
    pt->a = Vec(0, 0, 0);
  }
}

// Calculate the total kinetic energy of the system
float ForceDirected::totalEnergy() {
  auto energy = 0.0;
  for (auto pt : graph->nodes) {
    auto speed = pt->v.magnitude();
    energy += 0.5 * pt->m * speed * speed;
  }

  return energy;
}

void ForceDirected::tick(float timestep) {
  if (!stopped) {
    applyCoulombsLaw();
    applyHookesLaw();
    attractToCentre();
    // updateVelocity(timestep);
    updatePosition(timestep);

    if (totalEnergy() <= minEnergyThreshold) {
      stopped = true;

      originalPositions.clear();
      for (auto node : graph->nodes) {
        originalPositions.push_back(node->p);
      }
    }
  } else {
    // applySubtleMovement(0.03);
  }

  time += timestep;
}

void ForceDirected::start() {
  if (graph->nodes.size() == 0) {
    return;
  }

  tick(0.03);
}

void ForceDirected::applySubtleMovement(float) {
  for (size_t i = 0; i < graph->nodes.size(); ++i) {
    auto node = graph->nodes[i];
    const Vec &originalPos = originalPositions[i];

    // Calculate a subtle movement based on time and node's original position
    float dx = movementAmplitude * sin(movementFrequency * time + originalPos.x);
    float dy = movementAmplitude * cos(movementFrequency * time + originalPos.y);
    float dz = movementAmplitude * sin(movementFrequency * time + originalPos.z);

    // Apply the movement relative to the original position
    node->p.x = originalPos.x + dx;
    node->p.y = originalPos.y + dy;
    node->p.z = originalPos.z + dz;
  }
}
