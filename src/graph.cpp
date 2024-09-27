#include "graph.h"

Graph::Graph() {
  nodes = {};
  edges = {};
  nodeSet = {};
  adjacency = {};
  nextNodeId = 0;
  nextEdgeId = 0;
}

void Graph::addNode(Node *node) {
  if (nodeSet.count(node->id) == 0) {
    nodes.push_back(node);
  }

  nodeSet[node->id] = node;

  // this.notify();
}

void Graph::addEdge(Edge *edge) {
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

Node *Graph::newNode(/*data*/) {
  auto node = new Node(std::to_string(nextNodeId) /*, data*/);
  nextNodeId++;
  addNode(node);
  return node;
}

Edge *Graph::newEdge(Node *source, Node *target /*, data*/) {
  auto edge = new Edge(std::to_string(nextEdgeId), source, target /*, data*/);
  nextEdgeId++;
  addEdge(edge);
  return edge;
};

// find the edges from node1 to node2
std::vector<Edge *> Graph::getEdges(Node *node1, Node *node2) {
  if (adjacency.count(node1->id) > 0 && adjacency[node1->id].count(node2->id) > 0) {
    return adjacency[node1->id][node2->id];
  }

  return {};
}

// remove a node and it's associated edges from the graph
void Graph::removeNode(Node *) {
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

Edge *Graph::addEdge(std::pair<std::string, std::string> edge) {
  auto node1 = nodeSet[edge.first];
  if (node1 == nullptr) {
    throw "invalid node name: " + edge.first;
  }

  auto node2 = nodeSet[edge.second];
  if (node2 == nullptr) {
    throw "invalid node name: " + edge.second;
  }

  return newEdge(node1, node2);
}

void Graph::addEdges(std::vector<std::pair<std::string, std::string>> args) {
  // accepts variable number of arguments, where each argument
  // is a triple [nodeid1, nodeid2, attributes]
  for (size_t i = 0; i < args.size(); i++) {
    addEdge(args[i]);
  }
}

Node *Graph::addNode(std::string name) {
  auto node = new Node(name);
  addNode(node);
  return node;
}

void Graph::addNodes(std::vector<std::string> items) {
  // accepts variable number of arguments, where each argument
  // is a string that becomes both node identifier and label
  for (size_t i = 0; i < items.size(); i++) {
    addNode(items[i]);
  }
}
