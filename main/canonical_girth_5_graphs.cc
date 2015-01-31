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
    if(!filter->IsGirthNGraph(g)) {
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
  for (int i = 0; i < graphs.size(); ++i) {
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
    for (int j = 0; j < matrix.size(); ++j) {
      f << matrix[j] << std::endl;
    }
    f << std::endl;
  }
  f.close();

  int max_girth = 0;
  for (int i = 0; i < extremal.size(); ++i) {
    max_girth = std::max(max_girth, GetMaxGirth(*extremal[i]));
  }
  printf("(count,size,max_girth) =  (%d,%d,%d)\n",
         extremal.size(), max_edges, max_girth);
  return max_girth;
}
}  // namespace

int main() {
  const int max_order = 20;
  for (int order = 3; order < max_order; ++order) {
      GirthNGraph filter(10);
      CanonicalGraphGenerator gen(order, &filter);
      vector<Graph *> *graphs = new vector<Graph *>();
      printf("Order = %d\n", order);
      gen.GenerateGraphs(&graphs, true );
      int max_girth =
          ExportGraphsToFile("canonical_girth_5_order_temp.txt", *graphs);
  }
  // CanonicalGraphGenerator gen(13, new Girth5Graph());
  // vector<Graph *> *graphs = new vector<Graph *>();
  // gen.GenerateGraphs(&graphs, true );
  // ExportGraphsToFile("canonical_girth_5_order_temp.txt", *graphs);
  return 0;
}
