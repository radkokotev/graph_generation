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

const int kNumberOfVertices = 8;
const char kExportFileName[] = "temp_diamond_free_10.txt";

int64_t final_count = 0;

void ExportGraphsToFile(const string &filename, const vector<Graph *> &graphs) {
  std::ofstream f;
  f.open(filename, std::ios::app);
  for (size_t i = 0; i < graphs.size(); ++i) {
    vector<string> matrix;
    graphs[i]->GetAdjMatrix(&matrix);
    for (size_t j = 0; j < matrix.size(); ++j) {
      f << matrix[j] << std::endl;
    }
    f << std::endl;
  }
  f.close();
  return;
}

void ExportAllNonIsomorphicGraphsForSequence(const vector<int> &seq) {
  vector<Graph *> all_graphs;
  DiamondFreeGraph filter;

  SimpleGraphGenerator::GenerateAllUniqueGraphs(seq, &filter, &all_graphs);
  bool should_export = false;
  if (!all_graphs.empty()) {
    final_count += all_graphs.size();
    should_export = true;
  }

  if (should_export) {
    ExportGraphsToFile(kExportFileName, all_graphs);
  }

  while (!all_graphs.empty()) {
    delete all_graphs.back();
    all_graphs.pop_back();
  }
}

} // namespace

int main() {
  vector<vector<int>> seqs;
  std::clock_t start = std::clock();
  SimpleGraphGenerator::GenerateAllDegreeSequences(kNumberOfVertices, &seqs);
  printf("Number of degree sequences is %lu\n", seqs.size());
  for (size_t i = 0; i < seqs.size(); ++i) {
    ExportAllNonIsomorphicGraphsForSequence(seqs[i]);
  }
  printf("The number of unique connected diamond-free graphs "
         "of order %d is %ld\n",
         kNumberOfVertices, final_count);
  printf("Exporting all graphs to file %s\n", kExportFileName);
  printf("Time: %.3f ms\n",
         (std::clock() - start) / (double)(CLOCKS_PER_SEC) * 1000);
  return 0;
}
