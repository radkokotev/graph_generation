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

TEST(GraphUtilitiesTest, DiamondFreeGraphFilterTest) {
  DiamondFreeGraph filter;
  {
    vector<string> v({ "0111", "1011", "1101", "1110" });
    Graph g(v);
    EXPECT_FALSE(filter.IsNewGraphAcceptable(0, g));
    EXPECT_FALSE(filter.IsNewGraphAcceptable(1, g));
    EXPECT_FALSE(filter.IsNewGraphAcceptable(2, g));
    EXPECT_FALSE(filter.IsNewGraphAcceptable(3, g));
  }
  {
    vector<string> v({ "0111", "1001", "1001", "1110" });
    Graph g(v);
    vector<int> new_adj_vertices = {1};
    EXPECT_FALSE(filter.IsNewGraphAcceptable(0, new_adj_vertices, g));
    new_adj_vertices.clear();
    // No new vertices added. Assumes it was in a safe state.
    EXPECT_TRUE(filter.IsNewGraphAcceptable(0, new_adj_vertices, g));
  }
  {
    vector<string> v({ "0111", "1000", "1000", "1000" });
    Graph g(v);
    EXPECT_TRUE(filter.IsNewGraphAcceptable(0, g));
    EXPECT_TRUE(filter.IsNewGraphAcceptable(1, g));
    EXPECT_TRUE(filter.IsNewGraphAcceptable(2, g));
    EXPECT_TRUE(filter.IsNewGraphAcceptable(3, g));
  }
  {
    vector<string> v({ "0111", "1000", "1000", "1000" });
    Graph g(v);
    vector<int> new_adj_vertices = {0, 1, 2, 3};
    EXPECT_TRUE(filter.IsNewGraphAcceptable(0, new_adj_vertices, g));
    EXPECT_TRUE(filter.IsNewGraphAcceptable(1, new_adj_vertices, g));
    EXPECT_TRUE(filter.IsNewGraphAcceptable(2, new_adj_vertices, g));
    EXPECT_TRUE(filter.IsNewGraphAcceptable(3, new_adj_vertices, g));
    new_adj_vertices.clear();
    // Again, no new adjacent edges, so we assume we were in a safe state.
    EXPECT_TRUE(filter.IsNewGraphAcceptable(3, new_adj_vertices, g));
  }
}

} // namespace graph_utils
