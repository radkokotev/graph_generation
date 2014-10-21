// Static graph utilities

#ifndef GRAPH_UTILS_GRAPH_UTILITIES_H_
#define GRAPH_UTILS_GRAPH_UTILITIES_H_

#include "graph.h"

namespace graph_utils {

class DiamondFreeGraph {
public:
  DiamondFreeGraph();
  virtual ~DiamondFreeGraph() {}

  static bool IsDiamondFree(const Graph &g);
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GRAPH_UTILITIES_H_
