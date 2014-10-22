
#include "graph.h"

#include <cstring>
#include <set>
#include <queue>

namespace graph_utils {

Graph::Graph(int n) {
  size_ = n;
  adj_matrix_.reset((bool *)malloc(sizeof(bool) * n * n));
  memset(adj_matrix_.get(), 0, n * n);
}

Graph::Graph(const Graph& g) {
  size_ = g.size();
  adj_matrix_.reset((bool *)malloc(sizeof(bool) * size_ * size_));
  memcpy(adj_matrix_.get(), g.adj_matrix_.get(), sizeof(bool) * size_ * size_);
}

Graph::Graph(const vector<string> &adj_matrix) {
  const int n = adj_matrix.size();
  size_ = n;
  adj_matrix_.reset((bool *)malloc(sizeof(bool) * n * n));
  memset(adj_matrix_.get(), 0, n * n);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      adj_matrix_.get()[i * n + j] = adj_matrix[i][j] != '0';
    }
  }
}

bool Graph::HasEdge(int v1, int v2) const {
  return adj_matrix_.get()[v1 * size_ + v2];
}

void Graph::AddEdge(int v1, int v2) {
  adj_matrix_.get()[v1 * size_ + v2] = true;
  adj_matrix_.get()[v2 * size_ + v1] = true;
}

void Graph::RemoveEdge(int v1, int v2) {
  adj_matrix_.get()[v1 * size_ + v2] = false;
  adj_matrix_.get()[v2 * size_ + v1] = false;
}

void Graph::GetAdjMatrix(vector<string> *v) const {
  v->clear();
  for (int i = 0; i < size_; ++i) {
    string line = "";
    for (int j = 0; j < size_; ++j) {
      line += adj_matrix_.get()[i * size_ + j] ? "1" : "0";
    }
    v->push_back(line);
  }
}

int Graph::size() const { return size_; }

bool Graph::IsConnected() const {
  std::set<int> visited_nodes;
  std::queue<int> q;
  q.push(0);
  visited_nodes.insert(0);
  while (!q.empty()) {
    int cur_edge = q.front();
    q.pop();
    visited_nodes.insert(cur_edge);
    for (int neighbour = 0; neighbour < size(); ++neighbour) {
      if (HasEdge(cur_edge, neighbour) &&
          visited_nodes.find(neighbour) == visited_nodes.end()) {
        visited_nodes.insert(neighbour);
        q.push(neighbour);
      }
    }
  }
  return visited_nodes.size() == size();
}

Graph& Graph::operator=(const Graph &g) {
  size_ = g.size();
  adj_matrix_.reset((bool *)malloc(sizeof(bool) * size_ * size_));
  memcpy(adj_matrix_.get(), g.adj_matrix_.get(), sizeof(bool) * size_ * size_);
  return *this;
}

} // namespace graph_utils
