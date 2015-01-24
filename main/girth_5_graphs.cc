// A main program to generate all diamond free graphs with N vertices.

#include <stdio.h>
#include <vector>
#include <queue>
#include <ctime>
#include <iostream>
#include <fstream>

#include "graph_utils/girth_5_graph.h"
#include "graph_utils/graph.h"
#include "graph_utils/graph_generator.h"
#include "nauty_utils/nauty_wrapper.h"

using std::vector;
using std::queue;
using graph_utils::SimpleGraphGenerator;
using graph_utils::Graph;
using graph_utils::Girth5Graph;
using nauty_utils::IsomorphismChecker;

namespace {

const int kNumberOfVertices = 10;
const char kExportFileName[] = "girth_5_order_temp.txt";

int64_t final_count = 0;
int64_t all_graphs_count = 0;
int64_t all_connected_diamond_free_count = 0;


bool IsGraphExtremal(const Graph& g) {
  int kExtremalSizes[] = {0, 0, 1, 2, 3, 5, 6, 8, 10, 12, 15, 16, 18, 21, 23,
                          26, 28, 31, 34, 38, 41, 44, 47, 50, 54, 57, 61, 65,
                          68, 72, 76};
  return kExtremalSizes[g.size()] == g.GetNumberOfEdges();
}

void ExportGraphsToFile(const string &filename, const vector<Graph *> &graphs) {
  std::ofstream f;
  f.open(filename, std::ios::app);
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
  Girth5Graph filter;
  SimpleGraphGenerator::GenerateAllUniqueGraphs(seq, &filter, &all_graphs);
  bool should_export = false;
  vector<Graph *> extremal_graphs;
  for (int i = 0; i < all_graphs.size(); ++i) {
    if (IsGraphExtremal(*all_graphs[i])) {
      extremal_graphs.push_back(all_graphs[i]);
    }
  }
  if (!extremal_graphs.empty()) {
    final_count += extremal_graphs.size();
    should_export = true;
  }

  if (should_export) {
    ExportGraphsToFile(kExportFileName, extremal_graphs);
  }

  while (!all_graphs.empty()) {
    delete all_graphs.back();
    all_graphs.pop_back();
  }
}

} // namespace

int main() {
  vector<vector<int> > seqs;
  std::clock_t start = std::clock();
  SimpleGraphGenerator::GenerateAllDegreeSequences(kNumberOfVertices, &seqs);
  printf("Total seqs are %d\n", seqs.size());
  for (int i = 0; i < seqs.size(); ++i) {
    if (i % 100 == 0)
      printf("Done with %d\n", i);
    if (i > 42000 && i % 10 == 0)
      printf("Done with %d\n", i);
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
