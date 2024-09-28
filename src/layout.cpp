#include "layout.h"

ForceDirected::ForceDirected( //
    Graph *graph,             //
    float stiffness,          //
    float repulsion,          //
    float damping,            //
    float minEnergyThreshold, //
    float maxSpeed            //
) {

  this->graph = graph;
  this->damping = damping;                       // velocity damping factor
  this->maxSpeed = maxSpeed;                     // nodes aren't allowed to exceed this speed
  this->repulsion = repulsion;                   // repulsion constant
  this->stiffness = stiffness;                   // spring stiffness constant
  this->minEnergyThreshold = minEnergyThreshold; // threshold used to determine render stop

  nodePoints = {};  // keep track of points associated with nodes
  edgeSprings = {}; // keep track of springs associated with edges
}

Point *ForceDirected::point(Node *node) {
  if (nodePoints.count(node->id) == 0) {
    auto mass = 1.0;
    // auto mass = (node.data.mass != undefined) ? node.data.mass : 1.0;
    nodePoints[node->id] = new Point(Vec::random(), mass);
  }

  return nodePoints[node->id];
}

Spring *ForceDirected::spring(Edge *edge) {
  if (edgeSprings.count(edge->id) == 0) {
    auto length = /*(edge.data.length != undefined) ? edge.data.length :*/ 1.0;

    Spring *existingSpring = nullptr;

    auto from = graph->getEdges(edge->source, edge->target);

    for (auto e : from) {
      if (existingSpring == nullptr && edgeSprings.count(e->id)) {
        existingSpring = edgeSprings[e->id];
      }
    }

    if (existingSpring != nullptr) {
      return new Spring(existingSpring->point1, existingSpring->point2, 0.0, 0.0);
    }

    auto to = graph->getEdges(edge->target, edge->source);

    for (auto e : from) {
      if (existingSpring == nullptr && edgeSprings.count(e->id)) {
        existingSpring = edgeSprings[e->id];
      }
    }

    if (existingSpring != nullptr) {
      return new Spring(existingSpring->point2, existingSpring->point1, 0.0, 0.0);
    }

    edgeSprings[edge->id] = new Spring(this->point(edge->source), this->point(edge->target), length, this->stiffness);
  }

  return edgeSprings[edge->id];
}

// physics
void ForceDirected::applyCoulombsLaw() {
  for (auto n1 : graph->nodes) {
    auto point1 = point(n1);
    for (auto n2 : graph->nodes) {
      auto point2 = point(n2);
      if (point1 != point2) {
        auto d = point1->p->subtract(point2->p);
        auto distance = d->magnitude() + 0.1; // avoid massive forces at small distances (and divide by zero)
        auto direction = d->normalise();

        // apply force to each end point
        point1->applyForce(direction->multiply(repulsion)->divide(distance * distance * 0.5));
        point2->applyForce(direction->multiply(repulsion)->divide(distance * distance * -0.5));
      }
    }
  }
}

void ForceDirected::applyHookesLaw() {
  for (auto e : graph->edges) {
    auto s = spring(e);
    auto d = s->point2->p->subtract(s->point1->p); // the direction of the spring
    auto displacement = s->length - d->magnitude();
    auto direction = d->normalise();

    // apply force to each end point
    s->point1->applyForce(direction->multiply(s->k * displacement * -0.5));
    s->point2->applyForce(direction->multiply(s->k * displacement * 0.5));
  }
}

void ForceDirected::attractToCentre() {
  for (auto n : graph->nodes) {
    auto pt = point(n);
    auto direction = pt->p->multiply(-1.0);
    pt->applyForce(direction->multiply(repulsion / 50.0));
  }
}

void ForceDirected::updateVelocity(float timestep) {
  for (auto n : graph->nodes) {
    auto pt = point(n);
    // Is this, along with updatePosition below, the only places that your
    // integration code exist?
    pt->v = pt->v->add(pt->a->multiply(timestep))->multiply(damping);
    if (pt->v->magnitude() > maxSpeed) {
      pt->v = pt->v->normalise()->multiply(maxSpeed);
    }
    pt->a = new Vec(0, 0, 0);
  }
}

void ForceDirected::updatePosition(float timestep) {
  for (auto n : graph->nodes) {
    auto pt = point(n);
    // Same question as above; along with updateVelocity, is this all of
    // your integration code?
    pt->p = pt->p->add(pt->v->multiply(timestep));
  }
}

void ForceDirected::tick(float timestep) {
  applyCoulombsLaw();
  applyHookesLaw();
  attractToCentre();
  updateVelocity(timestep);
  updatePosition(timestep);
}

// Calculate the total kinetic energy of the system
float ForceDirected::totalEnergy() {
  auto energy = 0.0;
  for (auto node : graph->nodes) {
    auto pt = point(node);
    auto speed = pt->v->magnitude();
    energy += 0.5 * pt->m * speed * speed;
  }

  return energy;
}

void ForceDirected::start() { tick(0.03); }
