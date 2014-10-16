
#include "nauty_wrapper.h"

#include <vector>
#include "nauty/nauty.h"

namespace nauty_utils {

IsomorphismChecker::IsomorphismChecker(bool keep_isomorphs) {
  keep_isomorphs_ = keep_isomorphs;
}

void IsomorphismChecker::AddGraphToCheck(Graph *g) {
  for (uint i = 0; i < graphs_.size(); ++i) {
    if (AreIsomorphic(*graphs_[i], *g)) {
      return;
    }
  }
  graphs_.push_back(g);
}

void IsomorphismChecker::GetAllNonIsomorphicGraphs(vector<Graph *> *v) {
  v->insert(v->begin(), graphs_.begin(), graphs_.end());
}

bool IsomorphismChecker::AreIsomorphic(const Graph &graph_a,
                                       const Graph &graph_b) {
  if (graph_a.size() != graph_b.size()) {
    return false;
  }

  DYNALLSTAT(int, lab1, lab1_sz);
  DYNALLSTAT(int, lab2, lab2_sz);
  DYNALLSTAT(int, ptn, ptn_sz);
  DYNALLSTAT(int, orbits, orbits_sz);
  DYNALLSTAT(graph, g1, g1_sz);
  DYNALLSTAT(graph, g2, g2_sz);
  DYNALLSTAT(graph, cg1, cg1_sz);
  DYNALLSTAT(graph, cg2, cg2_sz);

  static DEFAULTOPTIONS_GRAPH(options);
  statsblk stats;
  options.getcanon = TRUE;

  int n = graph_a.size();
  int m = SETWORDSNEEDED(n);
  nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

  char *malloc_name = "malloc";
  DYNALLOC1(int, lab1, lab1_sz, n, malloc_name);
  DYNALLOC1(int, lab2, lab2_sz, n, malloc_name);
  DYNALLOC1(int, ptn, ptn_sz, n, malloc_name);
  DYNALLOC1(int, orbits, orbits_sz, n, malloc_name);
  DYNALLOC2(graph, g1, g1_sz, n, m, malloc_name);
  DYNALLOC2(graph, g2, g2_sz, n, m, malloc_name);
  DYNALLOC2(graph, cg1, cg1_sz, n, m, malloc_name);
  DYNALLOC2(graph, cg2, cg2_sz, n, m, malloc_name);

  // Make the graphs.

  EMPTYGRAPH(g1, m, n);
  EMPTYGRAPH(g2, m, n);

  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (graph_a.HasEdge(i, j)) {
        ADDONEEDGE(g1, i, j, m);
      }
      if (graph_b.HasEdge(i, j)) {
        ADDONEEDGE(g2, i, j, m);
      }
    }
  }

  densenauty(g1, lab1, ptn, orbits, &options, &stats, m, n, cg1);
  densenauty(g2, lab2, ptn, orbits, &options, &stats, m, n, cg2);

  return memcmp(cg1, cg2, m * sizeof(graph) * n) == 0;
}

} // nauty_utils
