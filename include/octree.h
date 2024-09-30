#pragma once

#include "node.h"
#include "vec.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////////

class Octree {
public:
  // Physical position/size. This implicitly defines the bounding box of this node
  Vec origin;          //! The physical center of this node
  float halfDimension; //! Half the width/height/depth of this node

  // The tree has up to eight children and can additionally store
  // a point, though in many applications only, the leaves will store data.
  Octree *children[8]; //! Pointers to child octants
  Node *data;          //! Data point to be stored at a node

  /*
    Children follow a predictable pattern to make accesses simple.
    Here, - means less than 'origin' in that dimension, + means greater than.
    child:	0 1 2 3 4 5 6 7
    x:      - - - - + + + +
    y:      - - + + - - + +
    z:      - + - + - + - +
   */

  Octree(const Vec &origin, float halfDimension) : origin(origin), halfDimension(halfDimension), data(NULL) {
    // Initially, there are no children
    for (int i = 0; i < 8; ++i)
      children[i] = nullptr;
  }

  // ~Octree() {
  //   // Recursively destroy octants
  //   for (int i = 0; i < 8; ++i)
  //     delete children[i];
  // }

  // Determine which octant of the tree would contain 'point'
  int getOctantContainingPoint(const Vec &point) const {
    int oct = 0;
    if (point.x >= origin.x)
      oct |= 4;
    if (point.y >= origin.y)
      oct |= 2;
    if (point.z >= origin.z)
      oct |= 1;
    return oct;
  }

  bool isLeafNode() const {
    // This is correct, but overkill. See below.
    /*
     for(int i=0; i<8; ++i)
     if(children[i] != NULL)
     return false;
     return true;
     */

    // We are a leaf iff we have no children. Since we either have none, or
    // all eight, it is sufficient to just check the first.
    return children[0] == NULL;
  }

  void insert(Node *point) {
    // If this node doesn't have a data point yet assigned
    // and it is a leaf, then we're done!
    if (isLeafNode()) {
      if (data == NULL) {
        data = point;
        return;
      } else {
        // We're at a leaf, but there's already something here
        // We will split this node so that it has 8 child octants
        // and then insert the old data that was here, along with
        // this new data point

        // Save this data point that was here for a later re-insert
        Node *oldPoint = data;
        data = NULL;

        // Split the current node and create new empty trees for each
        // child octant.
        for (int i = 0; i < 8; ++i) {
          // Compute new bounding box for this child
          Vec newOrigin = origin;
          newOrigin.x += halfDimension * (i & 4 ? .5f : -.5f);
          newOrigin.y += halfDimension * (i & 2 ? .5f : -.5f);
          newOrigin.z += halfDimension * (i & 1 ? .5f : -.5f);
          children[i] = new Octree(newOrigin, halfDimension * 0.5f);
        }

        // Re-insert the old point, and insert this new point
        // (We wouldn't need to insert from the root, because we already
        // know it's guaranteed to be in this section of the tree)
        children[getOctantContainingPoint(oldPoint->p)]->insert(oldPoint);
        children[getOctantContainingPoint(point->p)]->insert(point);
      }
    } else {
      // We are at an interior node. Insert recursively into the
      // appropriate child octant
      int octant = getOctantContainingPoint(point->p);
      children[octant]->insert(point);
    }
  }

  // This is a really simple routine for querying the tree for points
  // within a bounding box defined by min/max points (bmin, bmax)
  // All results are pushed into 'results'
  void getPointsInsideBox(const Vec &bmin, const Vec &bmax, std::vector<Node *> &results) {
    // If we're at a leaf node, just see if the current data point is inside
    // the query bounding box
    if (isLeafNode()) {
      if (data != NULL) {
        const Vec &p = data->p;
        if (p.x > bmax.x || p.y > bmax.y || p.z > bmax.z)
          return;
        if (p.x < bmin.x || p.y < bmin.y || p.z < bmin.z)
          return;
        results.push_back(data);
      }
    } else {
      // We're at an interior node of the tree. We will check to see if
      // the query bounding box lies outside the octants of this node.
      for (int i = 0; i < 8; ++i) {
        // Compute the min/max corners of this child octant
        Vec cmax = children[i]->origin + children[i]->halfDimension;
        Vec cmin = children[i]->origin - children[i]->halfDimension;

        // If the query rectangle is outside the child's bounding box,
        // then continue
        if (cmax.x < bmin.x || cmax.y < bmin.y || cmax.z < bmin.z)
          continue;
        if (cmin.x > bmax.x || cmin.y > bmax.y || cmin.z > bmax.z)
          continue;

        // At this point, we've determined that this child is intersecting
        // the query bounding box
        children[i]->getPointsInsideBox(bmin, bmax, results);
      }
    }
  }
};

////////////////////////////////////////////////////////////////////////////////

class OctreeNode {
public:
  Vec center;
  float size;
  Vec centerOfMass;
  int numNodes;
  Node *node;
  std::array<std::unique_ptr<OctreeNode>, 8> children;

  OctreeNode(const Vec &center, float size);

  void insert(Node *n);

private:
  void subdivide();

  void insertToChild(Node *n);
};

// // Driver code
// int main() {
//   Octree tree(1, 1, 1, 5, 5, 5);
//
//   tree.insert(1, 2, 3);
//   tree.insert(1, 2, 3);
//   tree.insert(6, 5, 5);
//
//   std::cout << (tree.find(1, 2, 3) ? "Found\n" : "Not Found\n");
//
//   std::cout << (tree.find(3, 4, 4) ? "Found\n" : "Not Found\n");
//   tree.insert(3, 4, 4);
//
//   std::cout << (tree.find(3, 4, 4) ? "Found\n" : "Not Found\n");
//
//   return 0;
// }

// Barnes-Hut algorithm implementation
// class BarnesHut {
// private:
//   std::unique_ptr<Octree> root;
//   float theta; // Barnes-Hut approximation parameter
//
// public:
//   BarnesHut(float theta = 0.5f) : theta(theta) {}
//
//   void buildTree(const std::vector<Node *> &nodes) {
//
//     // Find bounding box
//     Vec min(std::numeric_limits<float>::max());
//     Vec max(std::numeric_limits<float>::lowest());
//
//     for (const auto &node : nodes) {
//       min = Vec::min(min, node->p);
//       max = Vec::max(max, node->p);
//     }
//
//     Vec center = (min.add(max)).multiply(0.5f);
//     // float size = std::max({max.x - min.x, max.y - min.y, max.z - min.z});
//     float size = -10;
//     printf("Size: %f\n", size);
//
//     root = std::make_unique<Octree>(center, size);
//     for (const auto &node : nodes) {
//       root->insert(node);
//     }
//   }
//
//   void calculateForces(Node *node, float repulsion) { calculateForcesRecursive(node, root.get(), repulsion); }
//
// private:
//   void calculateForcesRecursive(Node *node, Octree *current, float repulsion) {
//     if (current->isLeafNode() == 0)
//       return;
//
//     Vec d = current->centerOfMass.subtract(node->p);
//     float distanceSquared = d.magnitude();
//
//     if (current->numNodes == 1 || (current->size * current->size / distanceSquared < theta * theta)) {
//       if (distanceSquared > 0) {
//         float distance = std::sqrt(distanceSquared);
//         Vec force = d.normalise().multiply((repulsion * current->numNodes / (distance * 0.5)));
//         node->applyForce(force);
//       }
//     } else {
//       for (size_t i = 0; i < 8; ++i) {
//         auto child = current->children[i];
//         if (child)
//           calculateForcesRecursive(node, child.get(), repulsion);
//       }
//     }
//   }
// };

class BarnesHut {
public:
  float theta;
  Octree *octree; // Pointer to the Octree

  BarnesHut(float theta = 0.5f) : theta(theta), octree(nullptr) {}

  void buildTree(const std::vector<Node *> &nodes) {
    // Create the root of the Octree based on the overall bounding box of the nodes
    Vec origin = calculateCentroid(nodes);
    float halfDimension = calculateHalfDimension(nodes);
    octree = new Octree(origin, halfDimension);

    // Insert all nodes into the Octree
    for (auto node : nodes) {
      octree->insert(node);
    }
  }

  void calculateForces(Node *targetNode, float repulsion) {
    // Traverse the Octree and calculate forces
    if (octree) {
      calculateForcesRecursive(octree, targetNode, repulsion);
    }
  }

private:
  Vec calculateCentroid(const std::vector<Node *> &nodes) {
    // Calculate the centroid of the nodes
    Vec sum(0, 0, 0);
    for (const auto &node : nodes) {
      sum = sum.add(node->p);
    }
    return sum.multiply(1.0f / nodes.size());
  }

  float calculateHalfDimension(const std::vector<Node *> &nodes) {
    if (nodes.empty()) {
      return 0.0f; // Return 0 if there are no nodes
    }

    // Initialize min and max values
    Vec minCoords = nodes[0]->p; // Start with the position of the first node
    Vec maxCoords = nodes[0]->p;

    // Iterate through all nodes to find the bounding box
    for (const auto &node : nodes) {
      minCoords.x = std::min(minCoords.x, node->p.x);
      minCoords.y = std::min(minCoords.y, node->p.y);
      minCoords.z = std::min(minCoords.z, node->p.z);

      maxCoords.x = std::max(maxCoords.x, node->p.x);
      maxCoords.y = std::max(maxCoords.y, node->p.y);
      maxCoords.z = std::max(maxCoords.z, node->p.z);
    }

    // Calculate the maximum dimension size
    float maxDimension = std::max({maxCoords.x - minCoords.x, maxCoords.y - minCoords.y, maxCoords.z - minCoords.z});

    return maxDimension * 0.5f; // Return half of the maximum dimension
  }

  void calculateForcesRecursive(Octree *node, Node *targetNode, float repulsion) {
    if (!node)
      return;

    // If this is a leaf node, compute the force
    if (node->isLeafNode()) {
      if (node->data && node->data != targetNode) {
        Vec direction = targetNode->p.subtract(node->data->p);
        float distanceSquared = direction.magnitudeSquared();
        if (distanceSquared > 0) {
          float distance = std::sqrt(distanceSquared);
          float forceMagnitude = repulsion / (distanceSquared); // Adjust force formula as needed
          Vec force = direction.normalise().multiply(forceMagnitude);
          targetNode->applyForce(force);
        }
      }
    } else {
      // If this is not a leaf, calculate the center of mass and size
      Vec centerOfMass = node->origin;      // Adjust based on your center of mass calculation
      float size = node->halfDimension * 2; // Diameter of the bounding box

      // Use a threshold to decide whether to approximate this node
      float distanceToCenter = targetNode->p.subtract(centerOfMass).magnitude();
      if (size / distanceToCenter < theta) { // theta is a threshold for approximation
        // Approximate: Apply force based on the center of mass
        Vec direction = targetNode->p.subtract(centerOfMass);
        float distanceSquared = direction.magnitudeSquared();
        if (distanceSquared > 0) {
          float distance = std::sqrt(distanceSquared);
          float forceMagnitude = repulsion / (distanceSquared); // Adjust force formula as needed
          Vec force = direction.normalise().multiply(forceMagnitude);
          targetNode->applyForce(force);
        }
      } else {
        // Recursively traverse child nodes
        for (int i = 0; i < 8; ++i) {
          calculateForcesRecursive(node->children[i], targetNode, repulsion);
        }
      }
    }
  }
};
