// A program to generate diamond-free graphs by using the canonical graph
// generator.
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"
#include "graph_utils/canonical_graph_generator.h"

using std::string;
using std::vector;
using graph_utils::Graph;
using graph_utils::CanonicalGraphGenerator;
using graph_utils::DiamondFreeGraph;

namespace {
void ExportGraphsToFile(const string &filename, const vector<Graph *> &graphs) {
  std::ofstream f;
  f.open(filename, std::ios::app);
  for (size_t i = 0; i < graphs.size(); ++i) {
    if (!graphs[i]->IsConnected()) {
      continue;
    }
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

} // namespace

int main() {
  const int kGraphOrder = 8;
  const string kFileName = "canonical_dfg_8.txt";
  CanonicalGraphGenerator gen(kGraphOrder, new DiamondFreeGraph());
  vector<Graph *> *graphs = new vector<Graph *>();
  printf("Generating diamond-free graphs of order %d\n", kGraphOrder);
  gen.GenerateGraphs(&graphs, true);
  ExportGraphsToFile("canonical_dfg_8.txt", *graphs);
  printf("Adjacency matrices for the final graphs are exported into the "
         "file: %s\n",
         kFileName.c_str());
  return 0;
}
