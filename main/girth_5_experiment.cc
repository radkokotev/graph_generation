// A main program to generate all diamond free graphs with N vertices.

#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"
#include "graph_utils/graph_generator.h"
#include "nauty_utils/nauty_wrapper.h"

using std::vector;
using std::queue;
using graph_utils::SimpleGraphGenerator;
using graph_utils::Graph;
using graph_utils::DiamondFreeGraph;
using nauty_utils::IsomorphismChecker;

namespace {

bool IsAcceptableGroupVertex(const int group,
                             const std::vector<int> &new_adj_vertices,
                             const Graph &g) {
  for (int i = 0; i < new_adj_vertices.size(); ++i) {
    if (new_adj_vertices[i] >= 8 + group * 4 &&
        new_adj_vertices[i] < 8 + (group + 1) * 4) {
      return false; // There is a triangle against removed vertices.
    }
  }
  for (int cur_v = 0; cur_v < 24; ++cur_v) {
    if (cur_v >= 8 + group * 4 && cur_v < 8 + (group + 1) * 4) {
      // Same group case has already been covered.
      continue;
    }
    bool connected_to_group = false;
    for (int v = 8 + group * 4; v < 8 + (group + 1) * 4; ++v) {
      if (g.HasEdge(cur_v, v)) {
        if (!connected_to_group) {
          connected_to_group = true;
        } else {
          return false;  // There is a square against removed vertices.
        }
      }
    }
  }
  return true;
}

class Girth5Filter : public graph_utils::GraphFilter {
public:
  Girth5Filter() {};
  virtual ~Girth5Filter() {}

  static bool IsGirth5(const Graph &g);

  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const Graph &g) const {
    return false;
  }

  virtual bool IsNewGraphAcceptable(const int cur_vertex,
                                    const std::vector<int> &new_adj_vertices,
                                    const Graph &g) const {
    if (cur_vertex < 8) {
      for (int group = 0; group < 4; ++group) {
        bool connected_to_group = false;
        for (int v = 8 + group * 4; v < 8 + (group + 1) * 4; ++v) {
          if (g.HasEdge(cur_vertex, v)) {
            if (!connected_to_group) {
              connected_to_group = true;
            } else {
              return false;  // There is a square.
            }
          }
        }
      }
    } else if (cur_vertex < 24) {
      const int group = cur_vertex / 4 - 2;
      if (!IsAcceptableGroupVertex(group, new_adj_vertices, g)) {
        return false;
      }
    } else {
      throw std::invalid_argument(
          "The graph is expected to have 24 vertices only.");
    }

    // Check for additional triangles and squares.
    for (int i = 0; i < new_adj_vertices.size(); ++i) {
      const int a = new_adj_vertices[i];
      for (int b = 0; b < g.size(); ++b) {
        if (b == a || b == cur_vertex) continue;
        if (g.HasEdge(cur_vertex, b) && g.HasEdge(a, b)) {
          return false;  // Triangle.
        }
        for (int c = b + 1; c < g.size(); ++c) {
          if (c == a || c == cur_vertex || !g.HasEdge(b, c)) continue;
          if ((g.HasEdge(cur_vertex, b) && g.HasEdge(a, c)) ||
              (g.HasEdge(cur_vertex, c) && g.HasEdge(a, b))) {
            return false;  // Square (i.e. cycle of length 4)
          }
        }
      }
    }

    return true;
  }
};

const int kNumberOfVertices = 24;
const char kExportFileName[] = "girth5_24.txt";

int64_t final_count = 0;
int64_t all_graphs_count = 0;
int64_t all_connected_diamond_free_count = 0;

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
  Girth5Filter filter;
  SimpleGraphGenerator::GenerateAllUniqueGraphs(seq, &filter, &all_graphs);
  bool should_export = false;
  if (!all_graphs.empty()) {
    final_count += all_graphs.size();
    should_export = true;
  }

  if (should_export) {
    ExportGraphsToFile(kExportFileName, all_graphs);
  }

  while (!all_graphs.empty()) {
    delete all_graphs.back();
    all_graphs.pop_back();
  }
}

} // namespace

int main() {
  vector<vector<int> > seqs;
  vector<int> desired_seq;
  for (int i = 0; i < 8; ++i) {
    desired_seq.push_back(5);
  }
  for (int i = 0; i < 16; ++i) {
    desired_seq.push_back(4);
  }
  seqs.push_back(desired_seq);
  std::clock_t start = std::clock();

  printf("Total seqs are %d\n", seqs.size());
  printf("Each seq is of size %d\n", seqs[0].size());
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
