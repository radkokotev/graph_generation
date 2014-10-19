// Tests for SimpleGraphGenerator.

#include <memory>
#include <vector>
#include <set>
#include <stdexcept>
#include <utility>

#include "graph_generator.h"
#include "gtest/gtest.h"

using std::vector;
using std::set;
using std::pair;
using std::make_pair;

namespace graph_utils {

class SimpleGraphGeneratorTest : public testing::Test {
protected:
  virtual void SetUp() { generator_.reset(new SimpleGraphGenerator()); }

  std::unique_ptr<SimpleGraphGenerator> generator_;
};

TEST_F(SimpleGraphGeneratorTest, DegreeSequence) {
  {
    vector<int> v({ 1, 1 });
    EXPECT_TRUE(SimpleGraphGenerator::IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 3, 3, 2, 2, 2, 2, 2, 2 });
    EXPECT_TRUE(SimpleGraphGenerator::IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 1, 1, 1 });
    EXPECT_FALSE(SimpleGraphGenerator::IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 4, 4, 2, 1, 1 });
    EXPECT_FALSE(SimpleGraphGenerator::IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 4, 4, 5, 5, 2 });
    // The degree sequence should be in non-increasing order.
    ASSERT_THROW(
        SimpleGraphGenerator::IsGraphicalDegreeSeq(v), std::invalid_argument);
  }
}

TEST_F(SimpleGraphGeneratorTest, ReducingDegreeSequence) {
  {
    vector<int> seq({ 1, 1 });
    vector<int> incident;
    // No vertices are incident on 1, but it has a degree of 1.
    ASSERT_THROW(SimpleGraphGenerator::ReduceDegreeSequence(
        0, incident, &seq), std::invalid_argument);
  }
  {
    vector<int> seq({ 1, 1 });
    vector<int> incident({ 2 });
    // Invalid vertex 2.
    ASSERT_THROW(SimpleGraphGenerator::ReduceDegreeSequence(
        0, incident, &seq), std::invalid_argument);
  }
  {
    vector<int> seq({ 1, 1 });
    vector<int> incident({ 1 }); // 1 is incident on 0.
    SimpleGraphGenerator::ReduceDegreeSequence(0, incident, &seq);
    ASSERT_EQ(2, seq.size());
    EXPECT_EQ(0, seq[0]);  // This vertex has been removed.
    EXPECT_EQ(0, seq[1]);  // This vertex was incident on the removed vertex.
  }
  {
    vector<int> seq({ 4, 4, 2, 1, 1 });  // Non-graphical sequence is fine.
    vector<int> incident({ 0, 2, 3, 4 });
    SimpleGraphGenerator::ReduceDegreeSequence(1, incident, &seq);
    ASSERT_EQ(5, seq.size());
    EXPECT_TRUE(seq[1] == 0 && seq[3] == 0 && seq[4] == 0);
    EXPECT_EQ(3, seq[0]);
    EXPECT_EQ(1, seq[2]);
  }
}

TEST_F(SimpleGraphGeneratorTest, ConstrainedGraphicalityTest) {
  {
    vector<pair<int, bool>> seq({ make_pair(3, true), make_pair(3, false),
        make_pair(2, true), make_pair(2, true), make_pair(2, true),
        make_pair(2, true), make_pair(2, true), make_pair(2, true) });
    EXPECT_TRUE(SimpleGraphGenerator::CGTest(0, seq));
  }
  {
    vector<pair<int, bool>> seq({ make_pair(2, true), make_pair(2, true),
        make_pair(1, true), make_pair(1, true) });
    EXPECT_TRUE(SimpleGraphGenerator::CGTest(0, seq));
    seq[2].second = false;
    EXPECT_TRUE(SimpleGraphGenerator::CGTest(0, seq));
    seq[3].second = false;
    EXPECT_FALSE(SimpleGraphGenerator::CGTest(0, seq));
  }
}

TEST_F(SimpleGraphGeneratorTest, GenerateAllAdjSetsTest) {
  vector<int> seq({ 3, 3, 2, 2, 2, 2, 2, 2 });
  set<int> cur_set;
  vector<set<int>> all_adj_sets;
  SimpleGraphGenerator::GenerateAllAdjSets(seq, &cur_set, &all_adj_sets);

  ASSERT_EQ(35, all_adj_sets.size());
  int entry = 0;
  for (int i = 7; i > 0; i--)
    for (int j = i - 1; j > 0; j--)
      for (int k = j - 1; k > 0; k--) {
        const set<int> &cur_set = all_adj_sets[entry++];
        int counter = 0;
        int expected[3] = {k, j, i};
        for (auto it = cur_set.cbegin(); it != cur_set.cend(); it++) {
          EXPECT_EQ(expected[counter++], *it);
        }
      }
}

TEST_F(SimpleGraphGeneratorTest, GenerateAllAdjSetsTest2) {
  vector<int> seq({ 2, 2, 1, 1 });
  set<int> cur_set;
  vector<set<int>> all_adj_sets;
  SimpleGraphGenerator::GenerateAllAdjSets(seq, &cur_set, &all_adj_sets);

  ASSERT_EQ(2, all_adj_sets.size());
  const set<int> &set1 = all_adj_sets[0];
  const set<int> &set2 = all_adj_sets[1];

  ASSERT_EQ(2, set1.size());
  // For this graph 0 and 1 need to be connected; 2 and 3 cannot be connected.
  auto it1 = set1.cbegin();
  EXPECT_EQ(1, *(it1++));
  EXPECT_EQ(3, *it1);

  ASSERT_EQ(2, set2.size());
  auto it2 = set2.cbegin();
  EXPECT_EQ(1, *(it2++));
  EXPECT_EQ(2, *it2);
}

} // namespace graph_utils
