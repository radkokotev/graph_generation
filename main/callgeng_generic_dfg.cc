// This is a sample of how to call geng as a procedure rather than running it as
// a separate process.  The basic idea is to construct an argument list for
// geng's main() function.  At compile time, assign a name to the macros OUTPROC
// and GENG_MAIN.  A typical Unix-style compilation command would be:
//   g++ -o callgeng_generic_girth -O3 -DMAXN=32 \
//      -DOUTPROC=myoutproc -DGENG_MAIN=geng_main -DPRUNE=geng_prune \
//      callgeng_generic_girth.cc geng.c girth_5_graph.o graph_utilities.o \
//      graph.o gtools.o nauty1.o nautil1.o naugraph1.o schreier.o naurng.o

#include <ctime>
#include <set>
#include <utility>
#include <vector>

#include "nauty/gtools.h"
#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"

using graph_utils::Graph;
using std::pair;

namespace {

graph_utils::DiamondFreeGraph filter;

// A helper function for converting from graph in NAUTY format to the graph
// format used in this project.
void ConvertNautyGraphToGraph(graph *g, int n, Graph *graph) {
  set *gi = (set*)g;
  const int m = (n + WORDSIZE - 1) / WORDSIZE;

  for (int i = 0; i < n - 1; ++i, gi += m) {
    for (int j = i+1; j < n; ++j) {
      if (ISELEMENT(gi,j)) {
        graph->AddEdge(i, j);
      }
    }
  }
}

void PrintGraphs(const std::vector<Graph *> graphs) {
  std::vector<std::string> matrix;
  for (int i = 0; i < graphs.size(); ++i) {
    graphs[i]->GetAdjMatrix(&matrix);
    for (int j = 0; j < matrix.size(); ++j) {
      printf("%s\n", matrix[j].c_str());
    }
    printf("\n");
    matrix.clear();
  }
}

}  // namespace

// Forward declaration of GENG_MAIN. The definition is resolved at link time
// from nauty/geng.c.
int GENG_MAIN(int argc, char *argv[]);

static unsigned long all_graphs;
static unsigned long counter;
static unsigned long max_size;
static std::vector<Graph *> extremal_graphs;

// Function which is called by GENG on final graph.
void OUTPROC(FILE *outfile, graph *g, int n) {
  ++all_graphs;
  Graph *graph = new Graph(n);
  ConvertNautyGraphToGraph(g, n, graph);
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
  if (argc != 2) {
    printf("Provide graph order as an argument.\n");
    return 1;
  }

  int geng_argc;
  char *geng_argv[6];

  // Set up geng argument list.  The 0-th argument is the command name.
  // There must be a NULL at the end.
  // TODO(radkokotev) optimize for cases, where geng provide functionality (e.g.
  // triangle- and/or square-free graphs with options -t and -f, respectively).
  geng_argv[0] = "geng";
  geng_argv[1] = "-q";      // suppress output from geng
  geng_argv[2] = "-c";      // connected graphs only
  geng_argv[3] = "-u";     // triangle- and square-free. Replace with -u.
  geng_argv[4] = argv[1];   // graph order
  geng_argv[5] = NULL;      // NULL
  geng_argc = 5;

  counter = 0;
  max_size = 0;
  std::clock_t start = std::clock();
  GENG_MAIN(geng_argc, geng_argv);

  printf("Total number of graphs = %lu, Number of extremal DFGs = %lu, "
         "size = %lu\n",
         all_graphs,
         counter,
         max_size);
  printf("Time: %.3f ms\n",
         (std::clock() - start) / (double)(CLOCKS_PER_SEC) * 1000);
  return 0;
}
