

#include <memory>
#include <vector>
#include <set>
#include <stdexcept>

#include "graph_generator.h"
#include "gtest/gtest.h"

using std::vector;
using std::set;

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
  vector<int> seq({ 3, 3, 2, 2, 2, 2, 2, 2 });
  set<int> forbidden({ 1 });
  EXPECT_TRUE(SimpleGraphGenerator::CGTest(0, forbidden, seq));
}

} // namespace graph_utils
