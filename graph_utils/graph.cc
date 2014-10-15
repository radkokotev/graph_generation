
#include "graph.h"

#include <cstring>

namespace graph_utils {

Graph::Graph(int n) {
  size = n;
  adj_matrix_.reset((bool *)malloc(sizeof(bool) * n * n));
  memset(adj_matrix_.get(), 0, n * n);
}

Graph::Graph(const vector<string> &adj_matrix) {
  const int n = adj_matrix.size();
  size = n;
  adj_matrix_.reset((bool *)malloc(sizeof(bool) * n * n));
  memset(adj_matrix_.get(), 0, n * n);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      adj_matrix_.get()[i * n + j] = adj_matrix[i][j] != '0';
    }
  }
}

bool Graph::HasEdge(int v1, int v2) {
  return adj_matrix_.get()[v1 * size + v2];
}

void Graph::AddEdge(int v1, int v2) {
  adj_matrix_.get()[v1 * size + v2] = true;
  adj_matrix_.get()[v2 * size + v1] = true;
}

void Graph::GetAdjMatrix(vector<string> *v) {
  v->clear();
  for (int i = 0; i < size; ++i) {
    string line = "";
    for (int j = 0; j < size; ++j) {
      line += adj_matrix_.get()[i * size + j] ? "1" : "0";
    }
    v->push_back(line);
  }
}

}  // namespace graph_utils
