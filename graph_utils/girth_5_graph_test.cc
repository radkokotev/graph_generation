// Tests for SimpleGraphGenerator.

#include <memory>
#include <vector>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include "girth_5_graph.h"
#include "graph_generator.h"
#include "graph_utilities.h"
#include "gtest/gtest.h"

using std::vector;
using std::set;
using std::pair;
using std::make_pair;
using std::string;

namespace graph_utils {
namespace {

template <typename T>
void ExpectVectorsEq(const vector<T> &v1, const vector<T> &v2) {
  ASSERT_EQ(v1.size(), v2.size());
  for (uint i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

} // namespace

class Girth5GraphTest : public testing::Test {
protected:
  virtual void SetUp() { filter_.reset(new Girth5Graph()); }

  std::unique_ptr<Girth5Graph> filter_;
};

TEST_F(Girth5GraphTest, NaiveGirth5Filter) {
  {
    vector<string> seq({ "01", "10" });
    Graph g(seq);
    EXPECT_TRUE(filter_->IsGirth5Graph(g));
  }
  {
    vector<string> seq({ "010", "101", "010" });
    Graph g(seq);
    EXPECT_TRUE(filter_->IsGirth5Graph(g));
  }
  {
    vector<string> seq({ "011", "101", "110" });
    Graph g(seq);
    EXPECT_FALSE(filter_->IsGirth5Graph(g));
  }
  {
    vector<string> seq({ "0100", "1010", "0101", "0010" });
    Graph g(seq);
    EXPECT_TRUE(filter_->IsGirth5Graph(g));
  }
  {
    vector<string> seq({ "0110", "1010", "1101", "0010" });
    Graph g(seq);
    EXPECT_FALSE(filter_->IsGirth5Graph(g));
  }
  {
    vector<string> seq({ "0101", "1010", "0101", "1010" });
    Graph g(seq);
    EXPECT_FALSE(filter_->IsGirth5Graph(g));
  }
  {
    vector<string> seq({ "01001", "10100", "01010", "00101", "10010" });
    Graph g(seq);
    EXPECT_TRUE(filter_->IsGirth5Graph(g));
  }
}

TEST_F(Girth5GraphTest, OptimizedGirth5Filter) {
  {
    vector<string> seq({ "010", "101", "010" });
    Graph g(seq);
    vector<int> adj = {2};
    EXPECT_TRUE(filter_->IsNewGraphAcceptable(1, adj, g));
  }
  {
    vector<string> seq({ "011", "101", "110" });
    Graph g(seq);
    vector<int> adj = {2};
    EXPECT_FALSE(filter_->IsNewGraphAcceptable(0, adj, g));
  }
  {
    vector<string> seq({ "0100", "1010", "0101", "0010" });
    Graph g(seq);
    vector<int> adj = {0, 2};
    EXPECT_TRUE(filter_->IsNewGraphAcceptable(1, adj, g));
  }
  {
    vector<string> seq({ "0110", "1010", "1101", "0010" });
    Graph g(seq);
    vector<int> adj = {0, 2};
    EXPECT_FALSE(filter_->IsNewGraphAcceptable(1, adj, g));
  }
  {
    vector<string> seq({ "0101", "1010", "0101", "1010" });
    Graph g(seq);
    vector<int> adj = {2};
    EXPECT_FALSE(filter_->IsNewGraphAcceptable(1, adj, g));
  }
  {
    vector<string> seq({ "0101", "1010", "0101", "1010" });
    Graph g(seq);
    vector<int> adj = {0, 2};
    EXPECT_FALSE(filter_->IsNewGraphAcceptable(3, adj, g));
  }
  {
    vector<string> seq({ "01001", "10100", "01010", "00101", "10010" });
    Graph g(seq);
    vector<int> adj = {2};
    EXPECT_TRUE(filter_->IsNewGraphAcceptable(1, adj, g));
  }
}

TEST_F(Girth5GraphTest, CanonicalFilterTest) {
  vector<string> seq({ "0100", "1010", "0101", "0010" });
  Graph g(seq);
  {
    vector<int> adj = {0};
    EXPECT_TRUE(filter_->IsSubsetSafe(g, adj));
  }
  {
    vector<int> adj = {0, 3};
    EXPECT_TRUE(filter_->IsSubsetSafe(g, adj));
  }
  {
    vector<int> adj = {0, 1};
    EXPECT_FALSE(filter_->IsSubsetSafe(g, adj));
  }
  {
    vector<int> adj = {0, 2};
    EXPECT_FALSE(filter_->IsSubsetSafe(g, adj));
  }
}

} // namespace graph_utils
