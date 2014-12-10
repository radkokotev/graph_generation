// Static graph utilities

#ifndef GRAPH_UTILS_GRAPH_UTILITIES_H_
#define GRAPH_UTILS_GRAPH_UTILITIES_H_

#include "graph.h"

namespace graph_utils {

// This is an interface, which should be implemented by every specific filter
// used by the brute-force graph generator.
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

// This abstract class should be extended by every specific filter for used by
// the canonical graph generator.
class CanonicalGraphFilter {
public:
  // Returns true if the given vertices in 'subset' is an appropriate subset of
  // vertices, which could be used to construct an upper object.
  // In more detail:
  //   A new vertex v' will be added to the graph g and connected to all
  //   vertices in 'subset'. The hypothetical new graph should also satisfy the
  //   property in mind. If it does not, this method should return false.
  virtual bool IsSubsetSafe(const Graph &g,
                            const std::vector<int> &subset) const = 0;

  // Creates all subsets of vertices for a graph of size 'n', which are later
  // used to create all upper graphs for a given graph. Basic implementation is
  // provided, no need to override. However, speed-ups are expected if subset
  // elimination is done in  a clever way here.
  virtual void GetAllSubsetOfVertices(
      const int n, std::vector<std::vector<int> *> *all_subsets) const;

  // This method creates a graph, which is the lower object of the given graph
  // 'g' by removing vertex 'v' from it, and all edges incident on 'v'.
  //
  // Ownership of the new graph is transferred to the caller. Basic
  // implementation is provided, no need to override.
  virtual void ReduceGraphByRemovingVertex(const Graph &g,
                                           const int v,
                                           Graph **result) const;
};

// Diamond free graph filter, which extends CanonicalGrapgFilter and implements
// GraphFilter could be used with both the brute-force graph generator and the
// canonical graph generator.
class DiamondFreeGraph : public CanonicalGraphFilter, public GraphFilter {
public:
  DiamondFreeGraph() {};
  virtual ~DiamondFreeGraph() {}

  // Override abstract method from CanonicalGraphFilter.
  virtual bool IsSubsetSafe(const Graph &g,
                            const std::vector<int> &subset) const;

  // Implement the two methods from GraphFilter interface.
  virtual bool IsNewGraphAcceptable(const int cur_vertex, const Graph &g) const;

  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const std::vector<int> &new_adj_vertices,
                                    const Graph &g) const;

  // Returns true if a the graph 'g' is diamond free (i.e. between any four
  // vertices there are at most four edges).
  static bool IsDiamondFree(const Graph &g);
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GRAPH_UTILITIES_H_
