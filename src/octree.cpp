#include "octree.h"

OctreeNode::OctreeNode(const Vec &center, float size)
    : center(center), size(size), centerOfMass({0, 0, 0}), numNodes(0) {}

void OctreeNode::insert(Node *n) {
  if (numNodes == 0) {
    node = n;
    centerOfMass = n->p;
    numNodes = 1;
  } else if (numNodes == 1) {
    subdivide();
    insertToChild(node);
    insertToChild(n);
    node = nullptr;
  } else {
    insertToChild(n);
  }

  centerOfMass = (centerOfMass.multiply(numNodes).add(n->p)).divide(numNodes + 1);
  numNodes++;
}

void OctreeNode::subdivide() {
  float childSize = size / 2;
  for (int i = 0; i < 8; i++) {
    float x = center.x + childSize * ((i & 1) ? 0.5f : -0.5f);
    float y = center.y + childSize * ((i & 2) ? 0.5f : -0.5f);
    float z = center.z + childSize * ((i & 4) ? 0.5f : -0.5f);
    children[i] = std::make_unique<OctreeNode>(Vec(x, y, z), childSize);
  }
}

void OctreeNode::insertToChild(Node *n) {
  int index = 0;
  if (n->p.x >= center.x)
    index |= 1;
  if (n->p.y >= center.y)
    index |= 2;
  if (n->p.z >= center.z)
    index |= 4;
  if (!children[index]) {
    children[index] = std::make_unique<OctreeNode>(Vec(0, 0, 0), size / 2);
  }
  children[index]->insert(n);
}
