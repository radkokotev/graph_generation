// A simple graph generator to enumarate all graphs with a given degree
// sequence. Based on Kim H.,  Toroczkai z., Erdos P., Miklos I., Szekely L.,
// "Degree-based graph construction" http://arxiv.org/pdf/0905.4892

#include <vector>

namespace graph_utils {

class SimpleGraphGenerator {
public:
  SimpleGraphGenerator();

  // Erdos-Gallai Theorem
  bool IsGraphicalDegreeSeq(const std::vector<int> &seq);
};

} // namespace graph_utils
