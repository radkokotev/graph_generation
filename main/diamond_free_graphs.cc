// A main program to generate all diamond free graphs with N vertices.

#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <ctime>
#include <iostream>
#include <fstream>

#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"
#include "graph_utils/graph_generator.h"
#include "nauty_utils/nauty_wrapper.h"

using std::vector;
using std::queue;
using graph_utils::SimpleGraphGenerator;
using graph_utils::Graph;
using graph_utils::DiamondFreeGraph;
using nauty_utils::IsomorphismChecker;

namespace {

const int kNumberOfVertices = 9;
const char kExportFileName[] = "diamond_free_9.txt";


int64_t final_count = 0;
int64_t all_graphs_count = 0;
int64_t all_connected_diamond_free_count = 0;

void ExportGraphsToFile(const string &filename,
                        const IsomorphismChecker &checker) {
  std::ofstream f;
  f.open (filename, std::ios::app);
  vector<Graph *> graphs;
  checker.GetAllNonIsomorphicGraphs(&graphs);
  for (int i = 0; i < graphs.size(); ++i) {
    vector<string> matrix;
    graphs[i]->GetAdjMatrix(&matrix);
    for (int j = 0; j < matrix.size(); ++j) {
      f << matrix[j] << std::endl;
    }
    f << std::endl;
  }
  f.close();
  return;
}

void ExportAllNonIsomorphicGraphsForSequence(const vector<int> &seq) {
  vector<Graph *> all_graphs;
  SimpleGraphGenerator::GenerateAllGraphs(seq, &all_graphs);
  all_graphs_count += all_graphs.size();

  bool should_export = false;
  IsomorphismChecker checker(true);
  for (int i = 0; i < all_graphs.size(); ++i) {
    if (all_graphs[i]->IsConnected() &&
        DiamondFreeGraph::IsDiamondFree(*(all_graphs[i]))) {
      ++all_connected_diamond_free_count;
      if (checker.AddGraphToCheck(all_graphs[i])) {
        ++final_count;
        should_export = true;
      }
    }
  }
  if (should_export) {
    ExportGraphsToFile(kExportFileName, checker);
  }

  while(!all_graphs.empty()) {
    delete all_graphs.back();
    all_graphs.pop_back();
  }
}

} // namespace

int main() {
  vector<vector<int>> seqs;
  std::clock_t start = std::clock();
  SimpleGraphGenerator::GenerateAllDegreeSequences(kNumberOfVertices, &seqs);
  printf("Total seqs are %d\n", seqs.size());
  for (int i = 0; i < seqs.size(); ++i) {
    if (i%1000 == 0) printf("Done with %d\n", i);
    ExportAllNonIsomorphicGraphsForSequence(seqs[i]);
  }

  printf("Number of vertices is %d\n", kNumberOfVertices);
  printf("The number of graph generated is %lld\n", all_graphs_count);
  printf("The number of connected diamond-free graphs is %lld\n",
         all_connected_diamond_free_count);
  printf("The number of unique connected diamond-free graphs is %lld\n",
         final_count);
  printf("Time: %.3f ms\n",
         (std::clock() - start) / (double)(CLOCKS_PER_SEC) * 1000);
  return 0;
}
