// A representation of a simple graph.
#ifndef GRAPH_UTILS_H_
#define GRAPH_UTILS_H_

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

namespace graph_utils {

class Graph {
public:
  Graph(int n);
  Graph(const vector<string> &adj_matrix);

  void AddEdge(int v1, int v2);
  bool HasEdge(int v1, int v2);
  void GetAdjMatrix(vector<string> *v);

private:
  int size;
  std::unique_ptr<bool> adj_matrix_;
};

}  // namespace graph_utils

#endif  // GRAPH_UTILS_H_
