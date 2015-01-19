#include "canonical_graph_generator.h"

#include <algorithm>
#include <ctime>
#include <math.h>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

#include "graph.h"
#include "graph_utilities.h"
#include "nauty_utils/nauty_wrapper.h"

using graph_utils::Graph;
using nauty_utils::IsomorphismChecker;
using std::set;
using std::vector;

namespace graph_utils {
namespace {

void DeleteVectorOfGraphs(vector<Graph *> *v) {
  while (!v->empty()) {
    delete v->back();
    v->pop_back();
  }
}

}  // namespace


CanonicalGraphGenerator::CanonicalGraphGenerator(const int n,
                                                 CanonicalGraphFilter *filter) {
  filter_ = filter;
  target_size_ = n;
}

void CanonicalGraphGenerator::GenerateUpperObjects(
    const Graph &g, vector<Graph *> *upper_obj) {
  const int n = g.size();
  vector<vector<int> *> all_subsets;
  filter_->GetAllSubsetOfVertices(n, &all_subsets);
  for (int i = 0; i < all_subsets.size(); ++i) {
    if (!filter_->IsSubsetSafe(g, *all_subsets[i])) {
      // Only safe sequences can be considered.
      continue;
    }
    // TODO(radkokotev) Improve implementation here.
    vector<string> matrix;
    g.GetAdjMatrix(&matrix);
    string newline = "";
    for (int j = 0; j < matrix.size(); ++j) {
      bool is_vertex_in_set =
          std::binary_search(all_subsets[i]->begin(), all_subsets[i]->end(), j);
      matrix[j] += is_vertex_in_set ? "1" : "0";
      newline += is_vertex_in_set ? "1" : "0";
    }
    newline += "0";  // graph is simple
    // Add the new line to the matrix and create the new graph.
    matrix.push_back(newline);
    upper_obj->push_back(new Graph(matrix));
  }
}

void CanonicalGraphGenerator::GenerateLowerObjects(
    const Graph &g, vector<Graph *> *lower_obj) {
  const int n = g.size();
  if (n <= 1) {
    return;
  }
  for (int i = 0; i < n; ++i) {
    // Reduce the graph g, by removing the vertex i and all edges incident on i.
    Graph *lower;
    filter_->ReduceGraphByRemovingVertex(g, i, &lower);
    lower_obj->push_back(lower);
  }
}

void CanonicalGraphGenerator::GetAllRelatedLowerObjects(
    const Graph &g, vector<Graph *> *lower_obj) {
  GenerateLowerObjects(g, lower_obj);
}

void CanonicalGraphGenerator::FindGraphsFromLowerObject(
    const Graph &lower_obj, vector<Graph *> *graphs) {
  vector<Graph *> candidates;
  GenerateUpperObjects(lower_obj, &candidates);
  for (int i = 0; i < candidates.size(); ++i) {
    vector<int> can_lab;
    IsomorphismChecker::GetCanonicalLabeling(*candidates[i], &can_lab);
    Graph *reduced;
    int vertex_to_remove =
        std::find(can_lab.begin(), can_lab.end(), 0) - can_lab.begin();
    filter_->ReduceGraphByRemovingVertex(
        *candidates[i], vertex_to_remove, &reduced);
    if (IsomorphismChecker::AreIsomorphic(lower_obj, *reduced)) {
      // TODO(radkokotev) Check only for automorphism.
      graphs->push_back(new Graph(*candidates[i]));
    }
    delete reduced;
  }
  while (!candidates.empty()) {
    delete candidates.back();
    candidates.pop_back();
  }
}


void CanonicalGraphGenerator::GenerateGraphs(vector<Graph *> **result,
                                             bool print_messages) {
  vector<Graph *> *cur = new vector<Graph *>();
  vector<Graph *> *next;

  Graph *k2 = new Graph(2);
  k2->AddEdge(0, 1);
  cur->push_back(k2);

  for (int n = 3; n <= target_size_; ++n) {
    next = new vector<Graph *>();
    std::clock_t start = std::clock();
    IsomorphismChecker checker(true);

    for (int graph_index = 0; graph_index < cur->size(); ++graph_index) {
      const Graph &g = *(*cur)[graph_index];
      vector<Graph *> upper_obj;
      GenerateUpperObjects(g, &upper_obj);
      for (int i = 0; i < upper_obj.size(); ++i) {
        vector<Graph *> related_lower_obj;
        GetAllRelatedLowerObjects(*upper_obj[i], &related_lower_obj);
        vector<Graph *> originals;
        for (int lower_index = 0;
            lower_index < related_lower_obj.size() && originals.empty();
            ++lower_index) {
          FindGraphsFromLowerObject(*related_lower_obj[lower_index],
                                    &originals);
        }
        DeleteVectorOfGraphs(&related_lower_obj);
        while (!originals.empty()) {
          if (!checker.AddGraphToCheck(originals.back())) {
            delete originals.back();
          }
          originals.pop_back();
        }
      }
      DeleteVectorOfGraphs(&upper_obj);
    }
    checker.GetAllNonIsomorphicGraphs(next);
    DeleteVectorOfGraphs(cur);
    delete cur;
    cur = next;

    if (print_messages) {
      int connected = 0;
      for (int i = 0; i < next->size(); ++i) {
        if ((*next)[i]->IsConnected()) {
          ++connected;
        }
      }
      printf("For n = %d there are in total %d graphs; connected -> %d",
             n, next->size(), connected);
      printf("  Time: %.3f ms\n",
           (std::clock() - start) / (double)(CLOCKS_PER_SEC) * 1000);
    }
  }
  *result = next;
}

} // namespace graph_utils
