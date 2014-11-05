
#ifndef NAUTY_UTILS_NAUTY_WRAPPER_H_
#define NAUTY_UTILS_NAUTY_WRAPPER_H_

#include <vector>
#include "graph_utils/graph.h"

namespace nauty_utils {

using std::vector;
using graph_utils::Graph;

class IsomorphismChecker {
public:
  IsomorphismChecker(bool keep_isomorphs);

  bool AddGraphToCheck(Graph *g);
  void GetAllNonIsomorphicGraphs(vector<Graph *> *v) const;
  static bool AreIsomorphic(const Graph &g1, const Graph &g2);
  static void GetCanonicalLabeling(const Graph &g, vector<int> *labels);

private:
  bool keep_isomorphs_;
  vector<Graph *> graphs_;
};

} // namespace nauty_utils

#endif // NAUTY_UTILS_NAUTY_WRAPPER_H_
