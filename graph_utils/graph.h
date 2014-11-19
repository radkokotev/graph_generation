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
  explicit Graph(const int n);
  explicit Graph(const Graph &g);
  explicit Graph(const vector<string> &adj_matrix);
  virtual ~Graph() {}

  virtual void AddEdge(const int v1, const int v2);
  virtual void RemoveEdge(const int v1, const int v2);
  virtual bool HasEdge(const int v1, const int v2) const;
  virtual void GetAdjMatrix(vector<string> *v) const;
  virtual int size() const;
  virtual bool IsConnected() const;
  virtual int GetNumberOfEdges() const;
  Graph &operator=(const Graph &g);

private:
  int size_;
  std::vector<bool> adj_matrix_;
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GRAPH_H_
