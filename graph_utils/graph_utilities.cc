
#include "graph_utilities.h"

#include "graph.h"

using std::vector;

namespace graph_utils {

void CanonicalGraphFilter::GetAllSubsetOfVertices(
    const int n, vector<vector<int> *> *all_subsets) const {
  // We know the size of the power set is 2^n.
  const long long int upper_bound = 1LL << n;
  // We shall discard the empty set from the generated set.
  for (long long int i = 1; i < upper_bound; ++i) {
    long long int mask = 0x01;
    vector<int> *cur_set = new vector<int>;
    int vertex = 0;
    for (int mask = 0x01, vertex = 0;
        mask <= i && vertex < n;
        ++vertex, mask <<= 1) {
      if (mask & i) {
        // if the current bit is 1 add the corresponding vertex to the vector.
        cur_set->push_back(vertex);
      }
    }
    all_subsets->push_back(cur_set);
  }
}

void CanonicalGraphFilter::ReduceGraphByRemovingVertex(const Graph &g,
                                                       const int v,
                                                       Graph **result) const {
  const int n = g.size();
  *result = new Graph(n - 1);
  for (int v1 = 0; v1 < n; ++v1) {
    if (v1 == v) {
      continue;
    }
    for (int v2 = v1 + 1; v2 < n; ++v2) {
      if (v2 == v || !g.HasEdge(v1, v2)) {
        // Only interested in edges which are not incident on v.
        continue;
      }
      // All vertices below the removed one keep the same "label", the ones
      // above are shifted down.
      const int a = v1 < v ? v1 : v1 - 1;
      const int b = v2 < v ? v2 : v2 - 1;
      (*result)->AddEdge(a, b);
    }
  }
}

bool DiamondFreeGraph::IsSubsetSafe(const Graph &g,
                                    const vector<int> &subset) const {
  const int n = g.size();
  for (int i = 0; i < subset.size(); ++i) {
    for (int j = i + 1; j < subset.size(); ++j) {
      for (int k = j + 1; k < subset.size(); ++k) {
        const int count_edges = (g.HasEdge(subset[i], subset[j]) ? 1 : 0) +
                                (g.HasEdge(subset[i], subset[k]) ? 1 : 0) +
                                (g.HasEdge(subset[j], subset[k]) ? 1 : 0);
        if (count_edges > 1) {
          return false;  // Three collinear vertices in the subset.
        }
      }
      for (int v = 0; v < n; ++v) {
        if (v == subset[i] || v == subset[j]) {
          continue;
        }
        if (g.HasEdge(subset[i], subset[j]) &&
            g.HasEdge(subset[i], v) &&
            g.HasEdge(subset[j], v)) {
          return false; // There is a triangle with two vertices in the subset.
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
    if (a == cur_vertex)
      continue;
    for (int b = a - 1; b >= 0; --b) {
      if (b == cur_vertex)
        continue;
      for (int c = b - 1; c >= 0; --c) {
        if (c == cur_vertex)
          continue;
        edge_count = (g.HasEdge(a, b) ? 1 : 0) + (g.HasEdge(a, c) ? 1 : 0) +
                     (g.HasEdge(a, cur_vertex) ? 1 : 0) +
                     (g.HasEdge(b, c) ? 1 : 0) +
                     (g.HasEdge(b, cur_vertex) ? 1 : 0) +
                     (g.HasEdge(c, cur_vertex) ? 1 : 0);
        if (edge_count > 4)
          return false; // The graph contains a diamond.
      }
    }
  }
  return true;
}

bool DiamondFreeGraph::IsNewGraphAcceptable(const int cur_vertex,
                                            const vector<int> &new_adj_vertices,
                                            const Graph &g) const {
  int edge_count;
  for (int i = 0; i < new_adj_vertices.size(); ++i) {
    const int a = new_adj_vertices[i];
    for (int b = g.size() - 1; b >= 0; --b) {
      if (b == cur_vertex || b == a)
        continue;
      for (int c = b - 1; c >= 0; --c) {
        if (c == cur_vertex || c == a)
          continue;
        edge_count = (g.HasEdge(a, b) ? 1 : 0) + (g.HasEdge(a, c) ? 1 : 0) +
                     (g.HasEdge(a, cur_vertex) ? 1 : 0) +
                     (g.HasEdge(b, c) ? 1 : 0) +
                     (g.HasEdge(b, cur_vertex) ? 1 : 0) +
                     (g.HasEdge(c, cur_vertex) ? 1 : 0);
        if (edge_count > 4)
          return false; // The graph contains a diamond.
      }
    }
  }
  return true;
}

bool DiamondFreeGraph::IsDiamondFree(const Graph &g) {
  int n = g.size();
  int edge_count;
  for (int a = n - 1; a >= 3; --a) {
    for (int b = a - 1; b >= 2; --b) {
      for (int c = b - 1; c >= 1; --c) {
        for (int d = c - 1; d >= 0; --d) {
          edge_count = (g.HasEdge(a, b) ? 1 : 0) + (g.HasEdge(a, c) ? 1 : 0) +
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
