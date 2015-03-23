
#include "nauty_wrapper.h"

#include <vector>
#include "nauty/nauty.h"

namespace nauty_utils {

IsomorphismChecker::IsomorphismChecker(bool optimize) { optimize_ = optimize; }

bool IsomorphismChecker::AddGraphToCheck(Graph *g) {
  vector<Graph *> *graphs;
  if (optimize_) {
    graphs = &(degree_to_graphs_[g->GetDegSeqString()]);
  } else {
    graphs = &graphs_;
  }
  for (size_t i = 0; i < graphs->size(); ++i) {
    if (AreIsomorphic(*(*graphs)[i], *g)) {
      return false;
    }
  }
  graphs->push_back(g);
  return true;
}

void IsomorphismChecker::GetAllNonIsomorphicGraphs(vector<Graph *> *v) const {
  if (!optimize_) {
    v->insert(v->begin(), graphs_.begin(), graphs_.end());
    return;
  }
  for (auto it = degree_to_graphs_.begin(); it != degree_to_graphs_.end();
       ++it) {
    v->insert(v->end(), it->second.begin(), it->second.end());
  }
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

  DEFAULTOPTIONS_GRAPH(options);
  statsblk stats;
  options.getcanon = TRUE;

  int n = graph_a.size();
  int m = SETWORDSNEEDED(n);
  nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

  DYNALLOC1(int, lab1, lab1_sz, n, "malloc");
  DYNALLOC1(int, lab2, lab2_sz, n, "malloc");
  DYNALLOC1(int, ptn, ptn_sz, n, "malloc");
  DYNALLOC1(int, orbits, orbits_sz, n, "malloc");
  DYNALLOC2(graph, g1, g1_sz, n, m, "malloc");
  DYNALLOC2(graph, g2, g2_sz, n, m, "malloc");
  DYNALLOC2(graph, cg1, cg1_sz, n, m, "malloc");
  DYNALLOC2(graph, cg2, cg2_sz, n, m, "malloc");

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

  bool result = memcmp(cg1, cg2, m * sizeof(graph) * n) == 0;

  DYNFREE(lab1, lab1_sz);
  DYNFREE(lab2, lab2_sz);
  DYNFREE(ptn, ptn_sz);
  DYNFREE(orbits, orbits_sz);
  DYNFREE(g1, g1_sz);
  DYNFREE(g2, g2_sz);
  DYNFREE(cg1, cg1_sz);
  DYNFREE(cg2, cg2_sz);

  return result;
}

void IsomorphismChecker::GetCanonicalLabeling(const Graph &g,
                                              vector<int> *labels) {
  DYNALLSTAT(int, lab1, lab1_sz);
  DYNALLSTAT(int, ptn, ptn_sz);
  DYNALLSTAT(int, orbits, orbits_sz);
  DYNALLSTAT(graph, g1, g1_sz);
  DYNALLSTAT(graph, cg1, cg1_sz);

  DEFAULTOPTIONS_GRAPH(options);
  statsblk stats;
  options.getcanon = TRUE;

  int n = g.size();
  int m = SETWORDSNEEDED(n);
  nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

  DYNALLOC1(int, lab1, lab1_sz, n, "malloc");
  DYNALLOC1(int, ptn, ptn_sz, n, "malloc");
  DYNALLOC1(int, orbits, orbits_sz, n, "malloc");
  DYNALLOC2(graph, g1, g1_sz, n, m, "malloc");
  DYNALLOC2(graph, cg1, cg1_sz, n, m, "malloc");

  EMPTYGRAPH(g1, m, n);
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (g.HasEdge(i, j)) {
        ADDONEEDGE(g1, i, j, m);
      }
    }
  }

  densenauty(g1, lab1, ptn, orbits, &options, &stats, m, n, cg1);

  for (int i = 0; i < n; ++i) {
    labels->push_back(lab1[i]);
  }

  DYNFREE(lab1, lab1_sz);
  DYNFREE(ptn, ptn_sz);
  DYNFREE(orbits, orbits_sz);
  DYNFREE(g1, g1_sz);
  DYNFREE(cg1, cg1_sz);
}

} // nauty_utils
