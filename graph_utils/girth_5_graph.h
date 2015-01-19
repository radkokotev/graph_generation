#ifndef GRAPH_UTILS_GIRTH_5_GRAPH_
#define GRAPH_UTILS_GIRTH_5_GRAPH_

#include <vector>

#include "graph.h"
#include "graph_utilities.h"

using std::vector;

namespace graph_utils {

// Girth 5 graph filter, which extends CanonicalGrapgFilter and implements
// GraphFilter could be used with both the brute-force graph generator and the
// canonical graph generator.
class Girth5Graph : public CanonicalGraphFilter, public GraphFilter {
public:
  Girth5Graph() {};
  virtual ~Girth5Graph() {}

  // Override abstract method from CanonicalGraphFilter.
  virtual bool IsSubsetSafe(const Graph &g,
                            const vector<int> &subset) const;

  // Implement the two methods from GraphFilter interface.
  virtual bool IsNewGraphAcceptable(const int cur_vertex, const Graph &g) const;

  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const vector<int> &new_adj_vertices,
                                    const Graph &g) const;

  // Returns true if a the graph 'g' is of girth 5 (i.e. there are not 3- and 4-
  // cycles).
  bool IsGirth5Graph(const Graph &g) const;
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GIRTH_5_GRAPH_
