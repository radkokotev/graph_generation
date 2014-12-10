
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
  for (int i = 0; i < graphs.size(); ++i) {
    if (!graphs[i]->IsConnected()) {
      continue;
    }
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
}  // namespace

int main() {
  CanonicalGraphGenerator gen(8, new DiamondFreeGraph());
  vector<Graph *> *graphs = new vector<Graph *>();
  gen.GenerateGraphs(&graphs, true );
  ExportGraphsToFile("canonical_dfg_8.txt", *graphs);
  return 0;
}
