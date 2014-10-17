// A simple graph generator to enumarate all graphs with a given degree
// sequence. Based on Kim H.,  Toroczkai z., Erdos P., Miklos I., Szekely L.,
// "Degree-based graph construction" http://arxiv.org/pdf/0905.4892

#include <vector>
#include <set>

namespace graph_utils {

class SimpleGraphGenerator {
public:
  SimpleGraphGenerator();

  // Erdos-Gallai Theorem
  static bool IsGraphicalDegreeSeq(const std::vector<int> &seq);

  // Returns true if the constrained graphicality test holds for the current
  // given set of connections.
  static bool CGTest(const int vertex,
                     const std::set<int> &forbidden,
                     const std::vector<int> &seq);

  // Reduces the degree sequence by removing 'vertex' from the graph and all
  // edges that are incident on that vertex.
  static void ReduceDegreeSequence(const int vertex,
                                   const std::vector<int> &incident_vertices,
                                   std::vector<int> *seq);
};

} // namespace graph_utils
