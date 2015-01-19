
#include <fstream>
#include "nauty_wrapper.h"
#include "gtest/gtest.h"

namespace nauty_utils {
namespace {

const char *kGraphsSize22FileName = "nauty_utils/testdata/F22_5_4.3.3.txt";
const char *kGraphsSize24FileName = "nauty_utils/testdata/mike24.txt";
const char *kGraphsSize32FileName = "nauty_utils/testdata/mike32.txt";

template <typename T>
void ExpectVectorsEq(const vector<T> &v1, const vector<T> &v2) {
  ASSERT_EQ(v1.size(), v2.size());
  for (uint i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

} // namespace

class IsomorphismCheckerTest : public testing::Test {
protected:
  virtual void SetUp() { checker_.reset(new IsomorphismChecker(true)); }

  void AddGraphToCheckFromFile(const string &filename) {
    std::ifstream infile(filename);
    string s;
    vector<string> mat;

    while ((infile >> s)) {
      if (!s.empty()) {
        mat.push_back(s);
      }
      if (s.size() == mat.size() && mat.size() > 0) {
        Graph *g = new Graph(mat);
        mat.clear();
        checker_->AddGraphToCheck(g);
      }
      ASSERT_LE(mat.size(), s.size());
    }
  }

  std::unique_ptr<IsomorphismChecker> checker_;
};

TEST_F(IsomorphismCheckerTest, OneGraph) {
  vector<string> v({ "000", "000", "000" });
  Graph g(v);
  ASSERT_TRUE(checker_->AddGraphToCheck(&g));
  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(1, graphs.size());
  vector<string> result_matrix;
  graphs[0]->GetAdjMatrix(&result_matrix);
  ExpectVectorsEq<string>(v, result_matrix);
}

TEST_F(IsomorphismCheckerTest, TwoNonIsomorphicGraphs) {
  vector<string> v1({ "000", "000", "000" });
  vector<string> v2({ "010", "100", "000" });
  Graph g1(v1);
  Graph g2(v2);
  ASSERT_TRUE(checker_->AddGraphToCheck(&g1));
  ASSERT_TRUE(checker_->AddGraphToCheck(&g2));

  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(2, graphs.size());
  {
    vector<string> result_matrix;
    graphs[0]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq<string>(v1, result_matrix);
  }
  {
    vector<string> result_matrix;
    graphs[1]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq<string>(v2, result_matrix);
  }
}

TEST_F(IsomorphismCheckerTest, TwoIsomorphicGraphs) {
  vector<string> v1({ "001", "000", "100" });
  vector<string> v2({ "010", "100", "000" });
  Graph g1(v1);
  Graph g2(v2);
  ASSERT_TRUE(checker_->AddGraphToCheck(&g1));
  ASSERT_FALSE(checker_->AddGraphToCheck(&g2));

  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(1, graphs.size());
  vector<string> result_matrix;
  graphs[0]->GetAdjMatrix(&result_matrix);
  ExpectVectorsEq<string>(v1, result_matrix);
}

TEST_F(IsomorphismCheckerTest, ThreeGraph_TwoIsomorphic) {
  vector<string> v1({ "000", "000", "000" });
  vector<string> v2({ "001", "000", "100" });
  vector<string> v3({ "010", "100", "000" });
  Graph g1(v1);
  Graph g2(v2);
  Graph g3(v3);
  ASSERT_TRUE(checker_->AddGraphToCheck(&g1));
  ASSERT_TRUE(checker_->AddGraphToCheck(&g2));
  ASSERT_FALSE(checker_->AddGraphToCheck(&g3));

  vector<Graph *> graphs;
  checker_->GetAllNonIsomorphicGraphs(&graphs);
  ASSERT_EQ(2, graphs.size());
  {
    vector<string> result_matrix;
    graphs[0]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq<string>(v1, result_matrix);
  }
  {
    vector<string> result_matrix;
    graphs[1]->GetAdjMatrix(&result_matrix);
    ExpectVectorsEq<string>(v2, result_matrix);
  }
}

TEST_F(IsomorphismCheckerTest, RealData_22) {
  AddGraphToCheckFromFile(kGraphsSize22FileName);
  vector<Graph *> result;
  checker_->GetAllNonIsomorphicGraphs(&result);
  // There are three non-isomorphic graphs in this file.
  EXPECT_EQ(3, result.size());
}

TEST_F(IsomorphismCheckerTest, RealData_24) {
  AddGraphToCheckFromFile(kGraphsSize24FileName);
  vector<Graph *> result;
  checker_->GetAllNonIsomorphicGraphs(&result);
  // There is one non-isomorphic graph in this file.
  EXPECT_EQ(1, result.size());
}

TEST_F(IsomorphismCheckerTest, RealData_32) {
  AddGraphToCheckFromFile(kGraphsSize32FileName);
  vector<Graph *> result;
  checker_->GetAllNonIsomorphicGraphs(&result);
  // There is one non-isomorphic graph in this file.
  EXPECT_EQ(1, result.size());
}

TEST_F(IsomorphismCheckerTest, CanonicalLabeling) {
  vector<string> v1({ "011", "100", "100" });
  vector<string> v2({ "001", "000", "100" });
  vector<string> v3({ "01110", "10101", "11000", "10001", "01010" });
  Graph g1(v1);
  Graph g2(v2);
  Graph g3(v3);
  {
    vector<int> labels;
    IsomorphismChecker::GetCanonicalLabeling(g1, &labels);
    vector<int> expected({ 1, 2, 0 });
    ExpectVectorsEq<int>(expected, labels);
  }
  {
    vector<int> labels;
    IsomorphismChecker::GetCanonicalLabeling(g2, &labels);
    vector<int> expected({ 1, 0, 2 });
    ExpectVectorsEq<int>(expected, labels);
  }
  {
    vector<int> labels;
    IsomorphismChecker::GetCanonicalLabeling(g3, &labels);
    vector<int> expected({ 3, 4, 2, 0, 1 });
    ExpectVectorsEq<int>(expected, labels);
  }
}

} // namespace nauty_utils
