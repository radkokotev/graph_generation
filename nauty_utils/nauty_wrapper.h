
#ifndef NAUTY_UTILS_NAUTY_WRAPPER_H_
#define NAUTY_UTILS_NAUTY_WRAPPER_H_

#include <map>
#include <vector>
#include "graph_utils/graph.h"

namespace nauty_utils {

using std::vector;
using std::map;
using graph_utils::Graph;

class IsomorphismChecker {
public:
  explicit IsomorphismChecker(bool optimize);

  bool AddGraphToCheck(Graph *g);
  void GetAllNonIsomorphicGraphs(vector<Graph *> *v) const;
  static bool AreIsomorphic(const Graph &g1, const Graph &g2);
  static void GetCanonicalLabeling(const Graph &g, vector<int> *labels);

private:
  bool optimize_;
  vector<Graph *> graphs_;
  map<int, vector<Graph *> > degree_to_graphs_;
};

} // namespace nauty_utils

#endif // NAUTY_UTILS_NAUTY_WRAPPER_H_
