
#include "gtest/gtest.h"
#include "graph.h"

namespace graph_utils {
namespace {

void ExpectVectorsEq(const vector<string> &v1, const vector<string> &v2) {
  ASSERT_EQ(v1.size(), v2.size());
  for (uint i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

} // namespace

TEST(GraphTest, NoEdges) {
  vector<string> v({ "000", "000", "000" });
  Graph g(v);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_FALSE(g.HasEdge(i, j));
    }
  }
}

TEST(GraphTest, SomeEdges) {
  vector<string> v({ "1111", "1111", "1111", "1111" });
  Graph g(v);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_TRUE(g.HasEdge(i, j));
    }
  }
}

TEST(GraphTest, AddEdges) {
  vector<string> v({ "000", "000", "000" });
  Graph g(v);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_FALSE(g.HasEdge(i, j));
    }
  }
  g.AddEdge(0, 1);
  g.AddEdge(0, 2);
  g.AddEdge(0, 3);
  for (int i = 1; i < 3; ++i) {
    for (int j = 1; j < 3; ++j) {
      EXPECT_FALSE(g.HasEdge(i, j));
    }
  }

  EXPECT_TRUE(g.HasEdge(0, 1) && g.HasEdge(0, 2) && g.HasEdge(0, 3) &&
              g.HasEdge(1, 0) && g.HasEdge(2, 0) && g.HasEdge(3, 0));
}

TEST(GraphTest, GetAdjMatrix) {
  vector<string> v({ "000", "000", "000" });
  Graph g(v);

  vector<string> mat1;
  vector<string> expected1({ "000", "000", "000" });
  g.GetAdjMatrix(&mat1);
  ExpectVectorsEq(expected1, mat1);

  g.AddEdge(0, 1);
  g.AddEdge(2, 2);
  vector<string> mat2;
  vector<string> expected2({ "010", "100", "001" });
  g.GetAdjMatrix(&mat2);
  ExpectVectorsEq(expected2, mat2);
}

TEST(GraphTest, NotConnected) {
  {
    vector<string> v({ "000", "000", "000" });
    Graph g(v);
    EXPECT_FALSE(g.IsConnected());
  }
  {
    vector<string> v({ "010", "100", "000" });
    Graph g(v);
    EXPECT_FALSE(g.IsConnected());
  }
}

TEST(GraphTest, Connected) {
  {
    vector<string> v({ "010", "101", "010" });
    Graph g(v);
    EXPECT_TRUE(g.IsConnected());
  }
  {
    vector<string> v({ "0111", "1011", "1101", "1110" });
    Graph g(v);
    EXPECT_TRUE(g.IsConnected());
  }
  {
    vector<string> v({"0101", "1010", "0100", "1000"});
    Graph g(v);
    EXPECT_TRUE(g.IsConnected());
  }
  {
    vector<string> v({"0110", "1001", "1000", "0100"});
    Graph g(v);
    EXPECT_TRUE(g.IsConnected());
  }
}

} // namespace grap_utils
