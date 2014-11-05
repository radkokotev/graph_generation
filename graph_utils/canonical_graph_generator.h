// Graph generator to generate graph of order n by following a canonical
// construction path. The implementation is based on McKAy's article:
// http://cs.anu.edu.au/~bdm/papers/orderly.pdf

#ifndef GRAPH_UTILS_CANONICAL_GRAPH_GENERATOR_H_
#define GRAPH_UTILS_CANONICAL_GRAPH_GENERATOR_H_

#include "graph.h"

namespace graph_utils {

class CanonicalGraphGenerator {
 public:
  // TODO(radkokotev) pass a generator, to generate all possible sets of
  // vertices W
  explicit CanonicalGraphGenerator(const int n);

  // Generates all upper objects <g, W> for the given graph g.
  void GenerateUpperObjects(const Graph &g, std::vector<Graph *> *upper_obj);

  // Generates all lower objects <g, v> for the given graph g. That is, for all
  // v in V(g) add the graphs g - v.
  void GenerateLowerObjects(const Graph &g, std::vector<Graph *> *lower_obj);

  // Implementation of f' mapping an upper object to a set of related lower
  // objects under the relation Rf.
  void GetAllRelatedLowerObjects(const Graph &g,
                                 std::vector<Graph *> *lower_obj);

  // For a given lower object finds back the possible original graphs.
  void FindGraphsFromLowerObject(const Graph &lower_obj,
                                std::vector<Graph *> *graphs);
};

} // namespace graph_utils

#endif // GRAPH_UTILS_CANONICAL_GRAPH_GENERATOR_H_
