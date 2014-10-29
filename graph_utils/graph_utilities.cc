
#include "graph_utilities.h"

#include "graph.h"

namespace graph_utils {

bool DiamondFreeGraph::IsDiamondFree(const Graph &g) {
  int n = g.size();
  int edge_count;
  for (int a = n - 1; a >= 3; --a) {
    for (int b = a - 1; b >= 2; --b) {
      for (int c = b - 1; c >= 1; --c) {
        for (int d = c - 1; d >= 0; --d) {
          edge_count =
              (g.HasEdge(a, b) ? 1 : 0) + (g.HasEdge(a, c) ? 1 : 0) +
              (g.HasEdge(a, d) ? 1 : 0) + (g.HasEdge(b, c) ? 1 : 0) +
              (g.HasEdge(b, d) ? 1 : 0) + (g.HasEdge(c, d) ? 1 : 0);
          if (edge_count > 4) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool DiamondFreeGraph::IsNewGraphAcceptable(const int cur_vertex,
                                            const Graph &g) const {
  int edge_count;
  for (int a = g.size() - 1; a >= 0; --a) {
    if (a == cur_vertex) continue;
    for (int b = a - 1; b >= 0; --b) {
      if (b == cur_vertex) continue;
      for (int c = b - 1; c >= 0; --c) {
        if (c == cur_vertex) continue;
        edge_count =
              (g.HasEdge(a, b) ? 1 : 0) + (g.HasEdge(a, c) ? 1 : 0) +
              (g.HasEdge(a, cur_vertex) ? 1 : 0) + (g.HasEdge(b, c) ? 1 : 0) +
              (g.HasEdge(b, cur_vertex) ? 1 : 0) +
              (g.HasEdge(c, cur_vertex) ? 1 : 0);
        if (edge_count > 4) return false;  // The graph contains a diamond.
      }
    }
  }
  return true;
}

bool DiamondFreeGraph::IsNewGraphAcceptable(
    const int cur_vertex,
    const std::vector<int> &new_adj_vertices,
    const Graph &g) const {
  int edge_count;
  for (int i = 0; i < new_adj_vertices.size(); ++i) {
    const int a = new_adj_vertices[i];
    for (int b = g.size() - 1; b >= 0; --b) {
      if (b == cur_vertex || b == a) continue;
      for (int c = b - 1; c >= 0; --c) {
        if (c == cur_vertex || c == a) continue;
        edge_count =
              (g.HasEdge(a, b) ? 1 : 0) + (g.HasEdge(a, c) ? 1 : 0) +
              (g.HasEdge(a, cur_vertex) ? 1 : 0) + (g.HasEdge(b, c) ? 1 : 0) +
              (g.HasEdge(b, cur_vertex) ? 1 : 0) +
              (g.HasEdge(c, cur_vertex) ? 1 : 0);
        if (edge_count > 4) return false;  // The graph contains a diamond.
      }
    }
  }
  return true;
}

} // namespace graph_utils
