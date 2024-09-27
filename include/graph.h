#pragma once

#include "edge.h"
#include "node.h"

#include <map>
#include <string>
#include <vector>

class Graph {
public:
  Graph();

  void addNode(Node *node);
  void addEdge(Edge *edge);
  Node *newNode(/*data*/);
  Edge *newEdge(Node *source, Node *target /*, data*/);
  Node *addNode(std::string name);
  Edge *addEdge(std::pair<std::string, std::string> edge);

  // find the edges from node1 to node2
  std::vector<Edge *> getEdges(Node *node1, Node *node2);

  // remove a node and it's associated edges from the graph
  void removeNode(Node *);
  void addEdges(std::vector<std::pair<std::string, std::string>> args);
  void addNodes(std::vector<std::string> items);

  int nextNodeId;
  int nextEdgeId;
  std::vector<Node *> nodes;
  std::vector<Edge *> edges;
  std::map<std::string, Node *> nodeSet;
  std::map<std::string, std::map<std::string, std::vector<Edge *>>> adjacency;
};
