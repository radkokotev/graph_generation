// This is a sample of how to call geng as a procedure rather than running it as
// a separate process.  The basic idea is to construct an argument list for
// geng's main() function.  At compile time, assign a name to the macros OUTPROC
// and GENG_MAIN.  A typical Unix-style compilation command would be:
//   g++ -o callgeng_generic_dfg -O3 -DMAXN=32
//      -DOUTPROC=myoutproc -DGENG_MAIN=geng_main -DPRUNE=geng_prune
//      callgeng_generic_dfg.cc geng.c girth_5_graph.o graph_utilities.o
//      graph.o gtools.o nauty1.o nautil1.o naugraph1.o schreier.o naurng.o

#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "nauty/gtools.h"
#include "graph_utils/graph.h"
#include "graph_utils/girth_5_graph.h"

using graph_utils::Graph;
using std::pair;
using std::string;

// Forward declaration of GENG_MAIN. The definition is resolved at link time
// from nauty/geng.c.
int GENG_MAIN(int argc, char *argv[]);

namespace {

static int all_graphs;
static int max_size;
static std::vector<Graph *> extremal_graphs;
static string output_file_name;

static int min_graph_girth = -1;
graph_utils::GirthNGraph *filter = nullptr;

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

// Returns the length of the shortest cycle in the graph by performing a
// level-order traversal.
int GetMinLengthCycle(const Graph &g) {
  const int kInfinity = 1000000;
  int min_length_cycle = kInfinity;
  for (int cur_vertex = 0; cur_vertex < g.size(); ++cur_vertex) {
    // a pair represents cur_vertex, source_vertex.
    std::unique_ptr<std::set<pair<int, int>>> curr_layer(
        new std::set<pair<int, int>>());
    std::unique_ptr<std::set<pair<int, int>>> next_layer(
        new std::set<pair<int, int>>());

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
            if (min_length_cycle > layers_traversed) {
              min_length_cycle = layers_traversed;
            }
            if (min_length_cycle == min_graph_girth) {
              return min_length_cycle;
            }
          }
          next_layer->insert(std::make_pair(j, vertex->first));
        }
      }
      curr_layer->clear();
      curr_layer.reset(next_layer.release());
      next_layer.reset(new std::set<pair<int, int>>());
      ++layers_traversed;
    }
  }
  return min_length_cycle == kInfinity ? 0 : min_length_cycle;
}

int MaxGirth(const std::vector<Graph *> graphs) {
  int max = 0;
  for (size_t i = 0; i < graphs.size(); ++i) {
    int smallest_cycle = GetMinLengthCycle(*graphs[i]);
    if (max < smallest_cycle) {
      max = smallest_cycle;
    }
  }
  return max;
}

void PrintGraphs(const std::vector<Graph *> graphs) {
  std::vector<std::string> matrix;
  for (size_t i = 0; i < graphs.size(); ++i) {
    graphs[i]->GetAdjMatrix(&matrix);
    for (size_t j = 0; j < matrix.size(); ++j) {
      printf("%s\n", matrix[j].c_str());
    }
    printf("\n");
    matrix.clear();
  }
}

} // namespace

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
    while (!extremal_graphs.empty()) {
      delete extremal_graphs.back();
      extremal_graphs.pop_back();
    }
    extremal_graphs.push_back(graph);
  } else if (edge_count == max_size) {
    extremal_graphs.push_back(graph);
  }
}

// Returns non-zero value if the graph should be rejected. Returns 0 if the
// graph has to be kept.
int PRUNE(graph *g, int n, int maxn) {
  Graph graph(n);
  ConvertNautyGraphToGraph(g, n, &graph);
  if (filter->IsGirthNGraph(graph)) {
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    printf("Usage:\n"
           "  ./callgeng_generic_girth.exe "
           "<graph_order> <min_girth> [<output_file>]\n\n"
           "Examples:\n"
           "  ./callgeng_generic_girth.exe 7 5\n"
           "      Produces all graphs of order 7 which contain no triangles "
           "and squares\n"
           "  ./callgeng_generic_girth.exe 7 4 temp.txt\n"
           "      Exports the adjacency matrices of all triangle-free graphs "
           "of order 7 into the file temp.txt. The file is appended to and not "
           "overwritten.\n");
    return 1;
  }

  sscanf(argv[2], "%d", &min_graph_girth);
  if (min_graph_girth < 3) {
    printf("The minimum graph girth must be an integer >= 3.\n");
  }
  filter = new graph_utils::GirthNGraph(min_graph_girth);
  char *geng_girth_option = nullptr;
  if (min_graph_girth == 3) {
    geng_girth_option = "-u"; // No constraint on graph girth.
  } else if (min_graph_girth == 4) {
    geng_girth_option = "-t"; // Graphs have to be triangle-free.
  } else {
    geng_girth_option = "-tf"; // Graphs have to be triangle- and square-free.
  }

  if (argc == 4) {
    output_file_name = argv[3];
  }

  // Set up geng argument list.  The 0-th argument is the command name.
  // There must be a NULL at the end.
  // TODO(radkokotev) optimize for cases, where geng provide functionality (e.g.
  // triangle- and/or square-free graphs with options -t and -f, respectively).
  int geng_argc = 5;
  char *geng_argv[6] = {"geng",
                        "-q", // suppress output from geng
                        "-c", // connected graphs only
                        geng_girth_option,
                        argv[1], // graph order
                        NULL};

  all_graphs = 0;
  max_size = 0;
  GENG_MAIN(geng_argc, geng_argv);

  int max_girth = MaxGirth(extremal_graphs);
  printf("Total number of graphs of girth at least %d on %s vertices is %d\n"
         "The number of extremal graphs, which contain no cycle of length %d "
         "or less, is %lu\n"
         "The size of the extremal graphs is %d\n"
         "The maximal girth among these extremal graphs is %d\n",
         min_graph_girth, argv[1], all_graphs, min_graph_girth - 1,
         extremal_graphs.size(), max_size, max_girth);

  return 0;
}
