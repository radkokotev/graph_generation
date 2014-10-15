
#include "nauty_wrapper.h"
#include "gtest/gtest.h"

namespace nauty_utils {
namespace {

void ExpectVectorsEq(const vector<string> &v1, const vector<string> v2) {
  ASSERT_EQ(v1.size(), v2.size());
  for (uint i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

} // namespace

class IsomorphismCheckerTest : public testing::Test {
protected:
  virtual void SetUp() { checker_.reset(new IsomorphismChecker(true)); }
  std::unique_ptr<IsomorphismChecker> checker_;
};

TEST_F(IsomorphismCheckerTest, OneGraph) {
  vector<string> v({ "000", "000", "000" });
  Graph g(v);
  checker_->AddGraphToCheck(&g);
  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(1, graphs.size());
  vector<string> result_matrix;
  graphs[0]->GetAdjMatrix(&result_matrix);
  ExpectVectorsEq(v, result_matrix);
}

TEST_F(IsomorphismCheckerTest, TwoNonIsomorphicGraphs) {
  vector<string> v1({ "000", "000", "000" });
  vector<string> v2({ "010", "100", "000" });
  Graph g1(v1);
  Graph g2(v2);
  checker_->AddGraphToCheck(&g1);
  checker_->AddGraphToCheck(&g2);

  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(2, graphs.size());
  {
    vector<string> result_matrix;
    graphs[0]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq(v1, result_matrix);
  }
  {
    vector<string> result_matrix;
    graphs[1]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq(v2, result_matrix);
  }
}

TEST_F(IsomorphismCheckerTest, TwoIsomorphicGraphs) {
  vector<string> v1({ "001", "000", "100" });
  vector<string> v2({ "010", "100", "000" });
  Graph g1(v1);
  Graph g2(v2);
  checker_->AddGraphToCheck(&g1);
  checker_->AddGraphToCheck(&g2);

  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(1, graphs.size());
  vector<string> result_matrix;
  graphs[0]->GetAdjMatrix(&result_matrix);
  ExpectVectorsEq(v1, result_matrix);
}

TEST_F(IsomorphismCheckerTest, ThreeGraph_TwoIsomorphic) {
  vector<string> v1({ "000", "000", "000" });
  vector<string> v2({ "001", "000", "100" });
  vector<string> v3({ "010", "100", "000" });
  Graph g1(v1);
  Graph g2(v2);
  Graph g3(v3);
  checker_->AddGraphToCheck(&g1);
  checker_->AddGraphToCheck(&g2);
  checker_->AddGraphToCheck(&g3);

  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(2, graphs.size());
  {
    vector<string> result_matrix;
    graphs[0]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq(v1, result_matrix);
  }
  {
    vector<string> result_matrix;
    graphs[1]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq(v2, result_matrix);
  }
}

} // namespace nauty_utils
