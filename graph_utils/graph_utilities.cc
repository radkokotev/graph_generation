
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

} // namespace graph_utils
