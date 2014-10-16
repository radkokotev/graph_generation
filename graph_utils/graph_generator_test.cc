

#include <memory>
#include <vector>
#include <stdexcept>

#include "graph_generator.h"
#include "gtest/gtest.h"

using std::vector;

namespace graph_utils {

class SimpleGraphGeneratorTest : public testing::Test {
protected:
  virtual void SetUp() { generator_.reset(new SimpleGraphGenerator()); }

  std::unique_ptr<SimpleGraphGenerator> generator_;
};

TEST_F(SimpleGraphGeneratorTest, DegreeSequence) {
  {
    vector<int> v({ 1, 1 });
    EXPECT_TRUE(generator_->IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 3, 3, 2, 2, 2, 2, 2, 2 });
    EXPECT_TRUE(generator_->IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 1, 1, 1 });
    EXPECT_FALSE(generator_->IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 4, 4, 2, 1, 1 });
    EXPECT_FALSE(generator_->IsGraphicalDegreeSeq(v));
  }
  {
    vector<int> v({ 4, 4, 5, 5, 2 });
    // The degree sequence should be in non-increasing order.
    ASSERT_THROW(generator_->IsGraphicalDegreeSeq(v), std::invalid_argument);
  }
}

} // namespace graph_utils
