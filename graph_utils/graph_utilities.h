// Static graph utilities

#ifndef GRAPH_UTILS_GRAPH_UTILITIES_H_
#define GRAPH_UTILS_GRAPH_UTILITIES_H_

#include "graph.h"

namespace graph_utils {

class GraphFilter {
 public:
  // This method assumes the graphs 'g' was just changed by adding edges
  // incident on 'cur_vertex'. This may help implementations reduce the number
  // of checks needed for a graph to satisfy this property.
  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const Graph &g) const = 0;

  // Same as above, but in addition the end points of the new edges are also
  // provided in 'new_adj_vertices'.
  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const std::vector<int> &new_adj_vertices,
                                    const Graph &g) const = 0;
};

class DiamondFreeGraph : public GraphFilter {
public:
  DiamondFreeGraph() {};
  virtual ~DiamondFreeGraph() {}

  static bool IsDiamondFree(const Graph &g);

  virtual bool IsNewGraphAcceptable(const int cur_vertex, const Graph &g) const;

  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const std::vector<int> &new_adj_vertices,
                                    const Graph &g) const;
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GRAPH_UTILITIES_H_
