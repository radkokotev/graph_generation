// A representation of a simple graph.
#ifndef GRAPH_UTILS_GRAPH_H_
#define GRAPH_UTILS_GRAPH_H_

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

namespace graph_utils {

class Graph {
public:
  Graph(int n);
  Graph(const Graph& g);
  Graph(const vector<string> &adj_matrix);
  virtual ~Graph() {}

  virtual void AddEdge(int v1, int v2);
  virtual void RemoveEdge(int v1, int v2);
  virtual bool HasEdge(int v1, int v2) const;
  virtual void GetAdjMatrix(vector<string> *v) const;
  virtual int size() const;
  Graph& operator=(const Graph &g);

private:
  int size_;
  std::unique_ptr<bool> adj_matrix_;
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GRAPH_H_
