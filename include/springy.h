#pragma once

#include <map>
#include <string>
#include <vector>

struct NodeData {
  struct {
    float r;
    float g;
    float b;
  } color;
};

class Node {
public:
  Node(std::string id /*, data*/) {
    this->id = id;
    // this.data = (data != = undefined) ? data : {};

    // Data fields used by layout algorithm in this file:
    // this.data.mass
    // Data used by default renderer in springyui.js
    // this.data.label
  };

  std::string id;
  NodeData data = {.color = {1.0f, 1.0f, 1.0f}};
};

class Edge {
public:
  Edge(std::string id, Node *source, Node *target /*, data */) {
    this->id = id;
    this->source = source;
    this->target = target;
    // this->data = (data != = undefined) ? data : {};

    // Edge data field used by layout alorithm
    // this.data.length
    // this.data.type
  }

  Node *source;
  Node *target;
  std::string id;
};

class Graph {
public:
  Graph() {
    nodes = {};
    edges = {};
    nodeSet = {};
    adjacency = {};
    nextNodeId = 0;
    nextEdgeId = 0;
  }

  void addNode(Node *node) {
    if (nodeSet.count(node->id) == 0) {
      nodes.push_back(node);
    }

    nodeSet[node->id] = node;

    // this.notify();
  }

  void addEdge(Edge *edge) {
    auto exists = false;
    for (auto e : edges) {
      if (edge->id == e->id) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      edges.push_back(edge);
    }

    if (adjacency.count(edge->source->id) == 0) {
      adjacency[edge->source->id] = {};
    }

    if (adjacency[edge->source->id].count(edge->target->id) == 0) {
      adjacency[edge->source->id][edge->target->id] = {};
    }

    exists = false;
    for (auto e : adjacency[edge->source->id][edge->target->id]) {
      if (edge->id == e->id) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      adjacency[edge->source->id][edge->target->id].push_back(edge);
    }

    // this.notify();
    // return edge;
  }

  Node *newNode(/*data*/) {
    auto node = new Node(std::to_string(nextNodeId) /*, data*/);
    nextNodeId++;
    addNode(node);
    return node;
  }

  Edge *newEdge(Node *source, Node *target /*, data*/) {
    auto edge = new Edge(std::to_string(nextEdgeId), source, target /*, data*/);
    nextEdgeId++;
    addEdge(edge);
    return edge;
  };

  // find the edges from node1 to node2
  std::vector<Edge *> getEdges(Node *node1, Node *node2) {
    if (adjacency.count(node1->id) > 0 && adjacency[node1->id].count(node2->id) > 0) {
      return adjacency[node1->id][node2->id];
    }

    return {};
  }

  // remove a node and it's associated edges from the graph
  void removeNode(Node *) {
    throw "unimplemented";
    // if (nodeSet.count(node->id) > 0) {
    //   delete nodeSet[node->id];
    // }

    // for (size_t i = nodes.size() - 1; i >= 0; i--) {
    //   if (nodes[i].id == node.id) {
    //     nodes.splice(i, 1);
    //   }
    // }

    // this.detachNode(node);
  }

  void addEdges(std::vector<std::pair<std::string, std::string>> args) {
    // accepts variable number of arguments, where each argument
    // is a triple [nodeid1, nodeid2, attributes]
    for (size_t i = 0; i < args.size(); i++) {
      auto e = args[i];
      auto node1 = nodeSet[e.first];
      if (node1 == nullptr) {
        throw "invalid node name: " + e.first;
      }

      auto node2 = nodeSet[e.second];
      if (node2 == nullptr) {
        throw "invalid node name: " + e.second;
      }

      newEdge(node1, node2);
    }
  }

  void addNodes(std::vector<std::string> items) {
    // accepts variable number of arguments, where each argument
    // is a string that becomes both node identifier and label
    for (size_t i = 0; i < items.size(); i++) {
      auto name = items[i];
      auto node = new Node(name);
      addNode(node);
    }
  }

  int nextNodeId;
  int nextEdgeId;
  std::vector<Node *> nodes;
  std::vector<Edge *> edges;
  std::map<std::string, Node *> nodeSet;
  std::map<std::string, std::map<std::string, std::vector<Edge *>>> adjacency;
};
