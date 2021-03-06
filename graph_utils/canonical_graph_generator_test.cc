// Unit tests for the canonical graph generator.

#include "canonical_graph_generator.h"

#include <stdio.h>
#include <math.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "graph.h"
#include "graph_utilities.h"
#include "gtest/gtest.h"
#include "nauty_utils/nauty_wrapper.h"

using std::set;
using std::vector;
using graph_utils::Graph;
using nauty_utils::IsomorphismChecker;

namespace graph_utils {
namespace {

template <typename T>
void ExpectVectorsEq(const vector<T> &v1, const vector<T> &v2) {
  ASSERT_EQ(v1.size(), v2.size());
  for (size_t i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

int CountConnectedGraphs(const vector<Graph *> &graphs) {
  int count = 0;
  for (size_t i = 0; i < graphs.size(); ++i) {
    if (graphs[i]->IsConnected()) {
      ++count;
    }
  }
  return count;
}

class AllGrapsAcceptable : public CanonicalGraphFilter {
public:
  virtual bool IsSubsetSafe(const Graph &g, const vector<int> &subset) const {
    return true;
  }
};

} // namespace

class CanonicalGraphGeneratorTest : public testing::Test {
protected:
  virtual void SetUp() { filter_.reset(new DiamondFreeGraph()); }
  std::unique_ptr<CanonicalGraphFilter> filter_;
};

TEST_F(CanonicalGraphGeneratorTest, UpperObjects_EmptyGraphTest) {
  vector<string> v({"000", "000", "000"});
  Graph g(v);
  CanonicalGraphGenerator generator(g.size(), filter_.get());
  vector<Graph *> upper_obj;
  generator.GenerateUpperObjects(g, &upper_obj);
  ASSERT_EQ(7, upper_obj.size());
  ASSERT_EQ(4, upper_obj[0]->size()); // The graphs are of order 4.
}

TEST_F(CanonicalGraphGeneratorTest, UpperObjects_TriangleGraphTest) {
  vector<string> v({"011", "101", "110"});
  Graph g(v);
  CanonicalGraphGenerator generator(g.size(), filter_.get());
  vector<Graph *> upper_obj;
  generator.GenerateUpperObjects(g, &upper_obj);
  ASSERT_EQ(3, upper_obj.size());
  ASSERT_EQ(4, upper_obj[0]->size()); // The graphs are of order 4.
  // Check each of the upper objects individually.
  {
    vector<string> expected({"0111", "1010", "1100", "1000"});
    vector<string> mat;
    upper_obj[0]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"0110", "1011", "1100", "0100"});
    vector<string> mat;
    upper_obj[1]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"0110", "1010", "1101", "0010"});
    vector<string> mat;
    upper_obj[2]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
}

TEST_F(CanonicalGraphGeneratorTest, UpperObjects_LineGraphTest) {
  vector<string> v({"010", "101", "010"});
  Graph g(v);
  CanonicalGraphGenerator generator(g.size(), filter_.get());
  vector<Graph *> upper_obj;
  generator.GenerateUpperObjects(g, &upper_obj);
  ASSERT_EQ(6, upper_obj.size());
  ASSERT_EQ(4, upper_obj[0]->size()); // The graphs are of order 4.
  // Check each of the upper objects individually.
  {
    vector<string> expected({"0101", "1010", "0100", "1000"});
    vector<string> mat;
    upper_obj[0]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"0100", "1011", "0100", "0100"});
    vector<string> mat;
    upper_obj[1]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"0101", "1011", "0100", "1100"});
    vector<string> mat;
    upper_obj[2]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }

  {
    vector<string> expected({"0100", "1010", "0101", "0010"});
    vector<string> mat;
    upper_obj[3]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"0101", "1010", "0101", "1010"});
    vector<string> mat;
    upper_obj[4]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"0100", "1011", "0101", "0110"});
    vector<string> mat;
    upper_obj[5]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
}

TEST_F(CanonicalGraphGeneratorTest, LowerObjectsTest) {
  vector<string> v({"0100", "1011", "0101", "0110"});
  Graph g(v);
  CanonicalGraphGenerator generator(g.size(), filter_.get());
  vector<Graph *> lower_obj;
  generator.GenerateLowerObjects(g, &lower_obj);
  ASSERT_EQ(4, lower_obj.size());
  // Check each of the lower objects individually.
  {
    vector<string> expected({"011", "101", "110"});
    vector<string> mat;
    lower_obj[0]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"000", "001", "010"});
    vector<string> mat;
    lower_obj[1]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"010", "101", "010"});
    vector<string> mat;
    lower_obj[2]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
  {
    vector<string> expected({"010", "101", "010"});
    vector<string> mat;
    lower_obj[3]->GetAdjMatrix(&mat);
    ExpectVectorsEq<string>(expected, mat);
  }
}

TEST_F(CanonicalGraphGeneratorTest, CompleteGeneration) {
  // Data as reported by McKay B.
  filter_.reset(new AllGrapsAcceptable());
  vector<Graph *> *result = nullptr;
  {
    CanonicalGraphGenerator generator(3, filter_.get());
    generator.GenerateGraphs(&result);
    ASSERT_EQ(2, CountConnectedGraphs(*result));
    delete result;
  }
  {
    CanonicalGraphGenerator generator(4, filter_.get());
    generator.GenerateGraphs(&result);
    ASSERT_EQ(6, CountConnectedGraphs(*result));
    delete result;
  }
  {
    CanonicalGraphGenerator generator(5, filter_.get());
    generator.GenerateGraphs(&result);
    ASSERT_EQ(21, CountConnectedGraphs(*result));
    delete result;
  }
  {
    CanonicalGraphGenerator generator(6, filter_.get());
    generator.GenerateGraphs(&result);
    ASSERT_EQ(112, CountConnectedGraphs(*result));
    delete result;
  }
}

} // namespace graph_utils
