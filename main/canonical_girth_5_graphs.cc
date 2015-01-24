#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "graph_utils/girth_5_graph.h"
#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"
#include "graph_utils/canonical_graph_generator.h"

using std::string;
using std::vector;
using graph_utils::Graph;
using graph_utils::CanonicalGraphGenerator;
using graph_utils::Girth5Graph;

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
  CanonicalGraphGenerator gen(10, new Girth5Graph());
  vector<Graph *> *graphs = new vector<Graph *>();
  gen.GenerateGraphs(&graphs, true );
  ExportGraphsToFile("canonical_girth_5_order_temp.txt", *graphs);
  return 0;
}
