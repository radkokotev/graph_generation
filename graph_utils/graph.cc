
#include "graph.h"

#include <algorithm>
#include <cstring>
#include <set>
#include <queue>

namespace graph_utils {

Graph::Graph(const int n) {
  size_ = n;
  adj_matrix_.resize(n * n, false);
}

Graph::Graph(const Graph &g) {
  size_ = g.size();
  adj_matrix_.insert(adj_matrix_.begin(), g.adj_matrix_.begin(),
                     g.adj_matrix_.end());
}

Graph::Graph(const vector<string> &adj_matrix) {
  const int n = adj_matrix.size();
  size_ = n;
  adj_matrix_.resize(n * n, false);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      adj_matrix_[i * n + j] = adj_matrix[i][j] != '0';
    }
  }
}

bool Graph::HasEdge(const int v1, const int v2) const {
  return adj_matrix_[v1 * size_ + v2];
}

void Graph::AddEdge(const int v1, const int v2) {
  adj_matrix_[v1 * size_ + v2] = true;
  adj_matrix_[v2 * size_ + v1] = true;
}

void Graph::RemoveEdge(const int v1, const int v2) {
  adj_matrix_[v1 * size_ + v2] = false;
  adj_matrix_[v2 * size_ + v1] = false;
}

void Graph::GetAdjMatrix(vector<string> *v) const {
  v->clear();
  for (int i = 0; i < size_; ++i) {
    string line = "";
    for (int j = 0; j < size_; ++j) {
      line += adj_matrix_[i * size_ + j] ? "1" : "0";
    }
    v->push_back(line);
  }
}

int Graph::size() const { return size_; }

bool Graph::IsConnected() const {
  std::set<int> visited_nodes;
  std::queue<int> q;
  q.push(0);
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


int Graph::GetNumberOfEdges() const {
  int count = 0;
  for (int i = 0; i < adj_matrix_.size(); ++i) {
    if (adj_matrix_[i]) {
      ++count;
    }
  }
  count /= 2;  // Assuming the graph is simple and there are not self edges.
  return count;
}

string Graph::GetDegSeqString() const {
  string result = "";
  for (int i = 0; i < adj_matrix_.size();) {
    int count = 0;
    for (int k = 0; k < size_; ++k, ++i) {
      if (adj_matrix_[i]) {
        ++count;
      }
    }
    result += std::to_string(count);
  }
  std::sort(result.begin(), result.end());
  return result;
}

Graph &Graph::operator=(const Graph &g) {
  size_ = g.size();
  adj_matrix_.insert(adj_matrix_.begin(), g.adj_matrix_.begin(),
                     g.adj_matrix_.end());
  return *this;
}

} // namespace graph_utils
