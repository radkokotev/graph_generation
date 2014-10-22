// Tests for SimpleGraphGenerator.

#include <string>
#include <vector>

#include "graph.h"
#include "graph_utilities.h"
#include "gtest/gtest.h"

using std::vector;
using std::string;

namespace graph_utils {

TEST(GraphUtilitiesTest, DiamondFreeGraphTest) {
  {
    vector<string> v({ "0111", "1011", "1101", "1110" });
    Graph g(v);
    EXPECT_FALSE(DiamondFreeGraph::IsDiamondFree(g));
  }
  {
    vector<string> v({ "0111", "1001", "1001", "1110" });
    Graph g(v);
    EXPECT_FALSE(DiamondFreeGraph::IsDiamondFree(g));
  }
  {
    vector<string> v({ "01111", "10101", "11000", "10001", "11010" });
    Graph g(v);
    EXPECT_FALSE(DiamondFreeGraph::IsDiamondFree(g));
  }
  {
    vector<string> v({ "0111", "1000", "1000", "1000" });
    Graph g(v);
    EXPECT_TRUE(DiamondFreeGraph::IsDiamondFree(g));
  }
  {
    vector<string> v({ "0111", "1001", "1000", "1100" });
    Graph g(v);
    EXPECT_TRUE(DiamondFreeGraph::IsDiamondFree(g));
  }
  {
    vector<string> v({ "01110", "10101", "11000", "10001", "01010" });
    Graph g(v);
    EXPECT_TRUE(DiamondFreeGraph::IsDiamondFree(g));
  }
}

} // namespace graph_utils
