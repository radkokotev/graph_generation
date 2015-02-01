// This is a sample of how to call geng as a procedure rather than running it as
// a separate process.  The basic idea is to construct an argument list for
// geng's main() function.  At compile time, assign a name to the macros OUTPROC
// and GENG_MAIN.  A typical Unix-style compilation command would be:
//   g++ -o callgeng_generic_girth -O3 -DMAXN=32 \
//      -DOUTPROC=myoutproc -DGENG_MAIN=geng_main -DPRUNE=geng_prune \
//      callgeng_generic_girth.cc geng.c girth_5_graph.o graph_utilities.o \
//      graph.o gtools.o nauty1.o nautil1.o naugraph1.o schreier.o naurng.o

#include <set>
#include <utility>
#include <vector>

#include "nauty/gtools.h"
#include "graph_utils/graph.h"
#include "graph_utils/girth_5_graph.h"

using graph_utils::Graph;
using std::pair;

namespace {

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

int GetMinLengthCycle(const Graph &g) {
  for (int cur_vertex = 0; cur_vertex < g.size(); ++cur_vertex) {
    // a pair represents cur_vertex, source_vertex.
    std::unique_ptr<std::set<pair<int,int> > > curr_layer(new std::set<pair<int,int> >());
    std::unique_ptr<std::set<pair<int,int> > > next_layer(new std::set<pair<int,int> >());

    curr_layer->insert(std::make_pair(cur_vertex, cur_vertex));
    int layers_traversed = 1;
    while (layers_traversed <= g.size()) {
      for (auto vertex = curr_layer->begin(); vertex != curr_layer->end();
          ++vertex) {
        for (int j = 0; j < g.size(); ++j) {
          // If this is the same vertex, or we are trying to go back, skip it.
          if (j == vertex->first || j == vertex->second ||
              !g.HasEdge(vertex->first, j)) {
            continue;
          }
          if (layers_traversed > 2 && j == cur_vertex) {
            return layers_traversed;
          }
          next_layer->insert(std::make_pair(j, vertex->first));
        }
      }
      curr_layer->clear();
      curr_layer.reset(next_layer.release());
      next_layer.reset(new std::set<pair<int,int> >());
      ++layers_traversed;
    }
  }
  return 0;
}

int MaxGirth(const std::vector<Graph *> graphs) {
  const int n = graphs[0]->size();
  int max = 0;
  for (int i = 0; i < graphs.size(); ++i) {
    int smallest_cycle = GetMinLengthCycle(*graphs[i]);
    if (max < smallest_cycle) {
      max = smallest_cycle;
    }
    // for (int girth = max >= 3 ? max + 1 : 3; girth < n; ++girth) {
    //   graph_utils::GirthNGraph cur_girth_filter(girth);
    //   if (!cur_girth_filter.IsGirthNGraph(*graphs[i]) && max < girth) {
    //     max = girth - 1;
    //     break;
    //   }
    // }
  }
  return max;
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

const int kGirth = 5;
graph_utils::GirthNGraph filter(kGirth);

}  // namespace

// Forward declaration of GENG_MAIN. The definition is resolved at link time
// from nauty/geng.c.
int GENG_MAIN(int argc, char *argv[]);

static unsigned long counter;
static unsigned long max_size;
static std::vector<Graph *> extremal_graphs;

// Function which is called by GENG on final graph.
void OUTPROC(FILE *outfile, graph *g, int n) {
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
  if (filter.IsGirthNGraph(graph)) {
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
  geng_argv[3] = "-tf";     // triangle- and square-free. Replace with -u.
  geng_argv[4] = argv[1];   // graph order
  geng_argv[5] = NULL;      // NULL
  geng_argc = 5;

  counter = 0;
  max_size = 0;
  GENG_MAIN(geng_argc, geng_argv);
  int max_girth = MaxGirth(extremal_graphs);
  printf("Number of graphs = %lu, size = %lu, max_girth = %d\n",
         counter,
         max_size,
         max_girth);
  if (max_girth > kGirth) {
    PrintGraphs(extremal_graphs);
  }
  return 0;
}
