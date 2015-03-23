// This is a program calls geng as a procedure in order to generate diamond-free
// graphs. At compile time, assign a name to the macros OUTPROC and GENG_MAIN.
// A typical Unix-style compilation command would be:
//   g++ -o callgeng_generic_girth -O3 -DMAXN=32
//      -DOUTPROC=myoutproc -DGENG_MAIN=geng_main -DPRUNE=geng_prune
//      callgeng_generic_girth.cc geng.c girth_5_graph.o graph_utilities.o
//      graph.o gtools.o nauty1.o nautil1.o naugraph1.o schreier.o naurng.o

#include <ctime>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "nauty/gtools.h"
#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"

using graph_utils::Graph;
using std::pair;
using std::string;

namespace {

graph_utils::DiamondFreeGraph filter;

// A helper function for converting from graph in NAUTY format to the graph
// format used in this project.
void ConvertNautyGraphToGraph(graph *g, int n, Graph *graph) {
  set *gi = (set *)g;
  const int m = (n + WORDSIZE - 1) / WORDSIZE;

  for (int i = 0; i < n - 1; ++i, gi += m) {
    for (int j = i + 1; j < n; ++j) {
      if (ISELEMENT(gi, j)) {
        graph->AddEdge(i, j);
      }
    }
  }
}

void ExportGraphToFile(const string &filename, const Graph &g) {
  std::ofstream f;
  f.open(filename, std::ios::app);
  vector<string> matrix;
  g.GetAdjMatrix(&matrix);
  for (size_t j = 0; j < matrix.size(); ++j) {
    f << matrix[j] << std::endl;
  }
  f << std::endl;
  f.close();
}

} // namespace

// Forward declaration of GENG_MAIN. The definition is resolved at link time
// from nauty/geng.c.
int GENG_MAIN(int argc, char *argv[]);

static int all_graphs;
static int counter;
static int max_size;
static std::vector<Graph *> extremal_graphs;
static string output_file_name;

// Function which is called by GENG on final graph.
void OUTPROC(FILE *outfile, graph *g, int n) {
  ++all_graphs;
  Graph *graph = new Graph(n);
  ConvertNautyGraphToGraph(g, n, graph);
  if (!output_file_name.empty()) {
    ExportGraphToFile(output_file_name, *graph);
  }
  int edge_count = graph->GetNumberOfEdges();
  if (edge_count > max_size) {
    max_size = edge_count;
    counter = 1;
    while (!extremal_graphs.empty()) {
      delete extremal_graphs.back();
      extremal_graphs.pop_back();
    }
    extremal_graphs.push_back(graph);
  } else if (edge_count == max_size) {
    ++counter;
    extremal_graphs.push_back(graph);
  }
}

// Returns non-zero value if the graph should be rejected. Returns 0 if the
// graph has to be kept.
int PRUNE(graph *g, int n, int maxn) {
  Graph graph(n);
  ConvertNautyGraphToGraph(g, n, &graph);
  if (filter.IsDiamondFree(graph)) {
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    printf("Usage:\n"
           "  ./callgeng_generic_dfg.exe <graph_order> [<output_file>]\n\n"
           "Examples:\n"
           "  ./callgeng_generic_dfg.exe 7\n"
           "      Produces all diamond-free graphs of order 7\n"
           "  ./callgeng_generic_dfg.exe 7 temp.txt\n"
           "      Exports the adjacency matrices of all diamond-free graphs of "
           "order 7 into the file temp.txt. The file is appended to and not "
           "overwritten.\n");
    return 1;
  }

  int geng_argc;
  char *geng_argv[6];

  // Set up geng argument list.  The 0-th argument is the command name.
  // There must be a NULL at the end.
  // TODO(radkokotev) optimize for cases, where geng provide functionality (e.g.
  // triangle- and/or square-free graphs with options -t and -f, respectively).
  geng_argv[0] = "geng";
  geng_argv[1] = "-q";    // suppress output from geng
  geng_argv[2] = "-c";    // connected graphs only
  geng_argv[3] = "-u";    // Only generate and counts graphs.
  geng_argv[4] = argv[1]; // graph order
  geng_argv[5] = NULL;    // NULL
  geng_argc = 5;

  counter = 0;
  max_size = 0;
  if (argc == 3) {
    output_file_name = argv[2];
  }
  std::clock_t start = std::clock();
  GENG_MAIN(geng_argc, geng_argv);

  printf("Total number of diamond-free graphs of order %s is %d\n"
         "Number of extremal DFGs = %d\n"
         "Size of extremal DFGs = %d\n",
         argv[1], all_graphs, counter, max_size);
  printf("Time: %.3f ms\n",
         (std::clock() - start) / (double)(CLOCKS_PER_SEC) * 1000);
  return 0;
}
