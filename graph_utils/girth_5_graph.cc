#include "girth_5_graph.h"

#include <set>
#include <utility>
#include <vector>

#include "graph.h"
#include "graph_utilities.h"

using std::pair;
using std::set;
using std::vector;

namespace graph_utils {

bool Girth5Graph::IsSubsetSafe(const Graph &g,
                               const vector<int> &subset) const {
  for (int i = 0; i < subset.size(); ++i) {
    int a = subset[i];
    for (int j = i + 1; j < subset.size(); ++j) {
      int b = subset[j];
      if (g.HasEdge(a, b)) {
        return false;  // triangle;
      }
      for (int c = 0; c < g.size(); ++c) {
        if (a == c || b == c) {
          continue;
        }
        if (g.HasEdge(a, c) && g.HasEdge(b, c)) {
          return false; // square.
        }
      }
    }
  }
  return true;
}

bool Girth5Graph::IsNewGraphAcceptable(const int cur_vertex,
                                       const Graph &g) const {
  const int girth = 5;
  // a pair represents cur_vertex, source_vertex.
  std::unique_ptr<set<pair<int,int> > > curr_layer(new set<pair<int,int> >());
  std::unique_ptr<set<pair<int,int> > > next_layer(new set<pair<int,int> >());

  curr_layer->insert(std::make_pair(cur_vertex, cur_vertex));
  int layers_traversed = 1;
  while (layers_traversed < girth) {
    for (auto vertex = curr_layer->begin(); vertex != curr_layer->end();
        ++vertex) {
      for (int j = 0; j < g.size(); ++j) {
        // If this is the same vertex, or we are trying to go back, skip it.
        if (j == vertex->first || j == vertex->second ||
            !g.HasEdge(vertex->first, j)) {
          continue;
        }
        if (layers_traversed > 2 && j == cur_vertex) {
          return false;
        }
        next_layer->insert(std::make_pair(j, vertex->first));
      }
    }
    curr_layer->clear();
    curr_layer.reset(next_layer.release());
    next_layer.reset(new set<pair<int,int> >());
    ++layers_traversed;
  }
}

bool Girth5Graph::IsNewGraphAcceptable(const int cur_vertex,
                                       const vector<int> &new_adj_vertices,
                                       const Graph &g) const {
  // Check for additional triangles and squares.
  for (int i = 0; i < new_adj_vertices.size(); ++i) {
    const int a = new_adj_vertices[i];
    for (int b = 0; b < g.size(); ++b) {
      if (b == a || b == cur_vertex) {
        continue;
      }
      if (g.HasEdge(cur_vertex, b) && g.HasEdge(a, b)) {
        return false;  // Triangle.
      }
      for (int c = b + 1; c < g.size(); ++c) {
        if (c == a || c == cur_vertex || !g.HasEdge(b, c)) {
          continue;
        }
        if ((g.HasEdge(cur_vertex, b) && g.HasEdge(a, c)) ||
            (g.HasEdge(cur_vertex, c) && g.HasEdge(a, b))) {
          return false;  // Square (i.e. cycle of length 4)
        }
      }
    }
  }
  return true;
}

bool Girth5Graph::IsGirth5Graph(const Graph &g) const {
  for (int i = 0; i < g.size(); ++i) {
    if (!IsNewGraphAcceptable(i, g)) {
      return false;
    }
  }
  return true;
}

} // namespace graph_utils

