// A program to generate graphs of minimum girth N.
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
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
using graph_utils::GirthNGraph;

namespace {

int GetMaxGirth(const Graph &g) {
  std::unique_ptr<GirthNGraph> filter;
  int max_girth = 0;
  for (int i = 3; i < g.size(); ++i) {
    filter.reset(new GirthNGraph(i));
    if (!filter->IsGirthNGraph(g)) {
      max_girth = i - 1;
      break;
    }
  }
  return max_girth;
}

int ExportGraphsToFile(const string &filename, const vector<Graph *> &graphs) {
  std::ofstream f;
  f.open(filename, std::ios::app);
  int max_edges = 0;
  vector<Graph *> extremal;
  for (size_t i = 0; i < graphs.size(); ++i) {
    if (!graphs[i]->IsConnected()) {
      continue;
    }
    int cur_edge_count = graphs[i]->GetNumberOfEdges();
    if (max_edges < cur_edge_count) {
      max_edges = cur_edge_count;
      extremal.clear();
      extremal.push_back(graphs[i]);
    } else if (max_edges == cur_edge_count) {
      extremal.push_back(graphs[i]);
    }
    vector<string> matrix;
    graphs[i]->GetAdjMatrix(&matrix);
    for (size_t j = 0; j < matrix.size(); ++j) {
      f << matrix[j] << std::endl;
    }
    f << std::endl;
  }
  f.close();

  int max_girth = 0;
  for (size_t i = 0; i < extremal.size(); ++i) {
    max_girth = std::max(max_girth, GetMaxGirth(*extremal[i]));
  }
  printf("(count,size,max_girth) =  (%lu,%d,%d)\n", extremal.size(), max_edges,
         max_girth);
  return max_girth;
}
} // namespace

int main() {
  const int kMaxOrder = 10;
  const int kMinGraphGirth = 10;
  printf("Generating graphs of minimum girth %d\n", kMinGraphGirth);
  for (int order = 3; order < kMaxOrder; ++order) {
    GirthNGraph filter(kMinGraphGirth);
    CanonicalGraphGenerator gen(order, &filter);
    vector<Graph *> *graphs = new vector<Graph *>();
    gen.GenerateGraphs(&graphs, true);
    const string filename = "results/canonical_girth_" +
                            std::to_string(kMinGraphGirth) + "_order_" +
                            std::to_string(order) + ".txt";
    ExportGraphsToFile(filename, *graphs);
    printf("Generated graphs of order %d. Exported to file %s\n", order,
           filename.c_str());
  }
  return 0;
}
