// Tests for SimpleGraphGenerator.

#include <memory>
#include <vector>
#include <stdexcept>

#include <string>

#include "graph.h"
#include "graph_utilities.h"
#include "graph_generator.h"
#include "nauty_utils/nauty_wrapper.h"
#include "gtest/gtest.h"

using std::vector;
using std::string;
using nauty_utils::IsomorphismChecker;

namespace graph_utils {

namespace {

void GenerateAllDegreeSequences(const int n,
                                vector<int> *cur_seq,
                                vector<vector<int>> *seqs) {
  if (cur_seq->size() >= n) {
    seqs->push_back(*cur_seq);
    return;
  }
  int m = n - 1;
  if (!cur_seq->empty()) {
    m = cur_seq->back();
  }

  cur_seq->push_back(0);
  for (int i = m; i > 0; --i) {
    (*cur_seq)[cur_seq->size() - 1] = i;
    GenerateAllDegreeSequences(n, cur_seq, seqs);
  }
  cur_seq->erase(cur_seq->begin() + cur_seq->size() - 1);
}

}

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

TEST(GraphUtilitiesTest, AllDiamondFreeGraphsTest) {
  vector<int> temp;
  vector<vector<int>> all;
  GenerateAllDegreeSequences(9, &temp, &all);
  printf("Count seqs for 3 is %d\n", all.size());
  int total_count = 0;
  for (int i = 0; i < all.size(); ++i) {
    SimpleGraphGenerator generator;
    // vector<int> seq({ 6, 6, 6, 3, 3, 3, 3, 3, 3 });
    const vector<int> &seq = all[i];
    vector<Graph> v;
    generator.GenerateAllGraphs(seq, &v);
    // printf("Initial size is %d\n", v.size());
    int count = 0;
    IsomorphismChecker checker(true);
    for (int i = 0; i < v.size(); ++i) {
      if (v[i].IsConnected() && DiamondFreeGraph::IsDiamondFree(v[i])) {
        ++count;
        checker.AddGraphToCheck(&v[i]);
      }
    }
    vector<Graph *> non_isomorphic;
    checker.GetAllNonIsomorphicGraphs(&non_isomorphic);
    // printf("Connected non-isomorphic graphs are %d, out of %d\n", non_isomorphic.size(), count);
    total_count += non_isomorphic.size();
    // for (int i = 0; i < non_isomorphic.size(); ++i) {
    //   printf("Graph number %d\n", i);
    //   vector<string> matrix;
    //   non_isomorphic[i]->GetAdjMatrix(&matrix);
    //   printf("Got the matrix of size %d\n", matrix.size());
    //   for (int j = 0; j < matrix.size(); ++j) {
    //     printf("%s\n", matrix[j].c_str());
    //   }
    //   printf("\n");
    // }
  }
  printf("Total count = %d\n", total_count);
}



} // namespace graph_utils
