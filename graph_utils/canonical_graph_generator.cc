
#include "canonical_graph_generator.h"

#include <stdio.h>
#include <math.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "graph.h"
#include "nauty_utils/nauty_wrapper.h"


using std::set;
using std::vector;
using graph_utils::Graph;
using nauty_utils::IsomorphismChecker;


namespace {
// TODO(radkokotev) these are temp definitions. Make the implementation generic.

// Generates all subsets of the set of vertices V (i.e. the power set of V).
// The caller is responsible to delete the vectors in all_subsets.
void GetAllSubsetOfVertices(const int n,
                            vector<vector<int> *> *all_subsets) {
  // We know the size of the power set is 2^n.
  const long long int upper_bound = static_cast<long long int>(pow(2, n));
  // We shall discard the empty set from the generated set.
  for (long long int i = 1; i < upper_bound; ++i) {
    long long int mask = 0x01;
    vector<int> *cur_set = new vector<int>;
    int vertex = 0;
    for (int mask = 0x01, vertex = 0;
        mask <= i && vertex < n;
        ++vertex, mask <<= 1) {
      if (mask & i) {
        // if the current bit is 1 add the corresponding vertex to the vector.
        cur_set->push_back(vertex);
      }
    }
    all_subsets->push_back(cur_set);
  }
}

// Generates all subsets of the set of vertices V (i.e. the power set of V).
// Canonically constructs the subsets. TODO(radkokotev) pruning is possible.
void GetAllSubsetOfVertices(const int n,
                            const int cur_set_size,
                            const vector<set<int> *> &prev_subsets,
                            set<set<int> > *all_subsets) {
  if (cur_set_size > n) {
    return;
  }
  vector<set<int> *> cur_sets;
  for (int i = 0; i < prev_subsets.size(); ++i) {
    set<int> *prev_set = prev_subsets[i];

    for (int new_vertex = 0; new_vertex < n; ++new_vertex) {
      if (prev_set->find(new_vertex) != prev_set->end()) {
        // This vertex is already in the subset. Not interested in adding it.
        continue;
      }
      set<int> *cur_subset = new set<int>(*prev_set);
      cur_subset->insert(new_vertex);
      if (all_subsets->find(*cur_subset) != all_subsets->end()) {
        // The set has already been added.
        delete cur_subset;
        continue;
      }
      all_subsets->insert(*cur_subset);
      cur_sets.push_back(cur_subset);
    }
  }
  GetAllSubsetOfVertices(n, cur_set_size + 1, cur_sets, all_subsets);
  while (!cur_sets.empty()) {
    delete cur_sets.back();
    cur_sets.pop_back();
  }
}

// Returns true if this is an appropriate subset of vertices that could be used
// to construct an upper object.
bool IsSubsetSafe(const Graph &g, const vector<int> &subset) {
  const int n = g.size();
  for (int i = 0; i < subset.size(); ++i) {
    for (int j = i + 1; j < subset.size(); ++j) {
      for (int k = j + 1; k < subset.size(); ++k) {
        const int count_edges = (g.HasEdge(subset[i], subset[j]) ? 1 : 0) +
                                (g.HasEdge(subset[i], subset[k]) ? 1 : 0) +
                                (g.HasEdge(subset[j], subset[k]) ? 1 : 0);
        if (count_edges > 1) {
          return false;
        }
      }
      for (int v = 0; v < n; ++v) {
        if (v == subset[i] || v == subset[j]) {
          continue;
        }
        if (g.HasEdge(subset[i], subset[j]) &&
            g.HasEdge(subset[i], v) &&
            g.HasEdge(subset[j], v)) {
          // There is a triangle, therefore is unsafe.
          return false;
        }
      }
    }
  }
  return true;
}

void ReduceGraphByRemovingVertex(const Graph &g, const int v, Graph **result) {
  const int n = g.size();
  *result = new Graph(n - 1);
  for (int v1 = 0; v1 < n; ++v1) {
    if (v1 == v) {
      continue;
    }
    for (int v2 = v1 + 1; v2 < n; ++v2) {
      if (v2 == v || !g.HasEdge(v1, v2)) {
        // Only interested in edges which are not incident on v.
        continue;
      }
      // All vertices below the removed one keep the same "label", the ones
      // above are shifted down.
      const int a = v1 < v ? v1 : v1 - 1;
      const int b = v2 < v ? v2 : v2 - 1;
      (*result)->AddEdge(a, b);
    }
  }
}

}  // namespace

namespace graph_utils {

CanonicalGraphGenerator::CanonicalGraphGenerator(const int n) {
  // TODO(radkokotev) provide implementation.
}

void CanonicalGraphGenerator::GenerateUpperObjects(
    const Graph &g, vector<Graph *> *upper_obj) {
  const int n = g.size();
  vector<vector<int> *> all_subsets;
  GetAllSubsetOfVertices(n, &all_subsets);
  for (int i = 0; i < all_subsets.size(); ++i) {
    if (!IsSubsetSafe(g, *all_subsets[i])) {
      // Only safe sequences can be considered.
      continue;
    }
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
    ReduceGraphByRemovingVertex(g, i, &lower);
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
    ReduceGraphByRemovingVertex(*candidates[i], vertex_to_remove, &reduced);
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




/*

int main2() {
  vector<set<int> *> prev_subsets;
  set<set<int> > all_subsets;
  set<int> empty;
  prev_subsets.push_back(&empty);
  GetAllSubsetOfVertices(8, 1, prev_subsets, &all_subsets);
  printf("The size of the set is %d\n", all_subsets.size());

  for (std::set<set<int> >::iterator i = all_subsets.begin();
      i != all_subsets.end();
      ++i) {
    printf("[");
    for (std::set<int>::iterator it = i->begin(); it != i->end(); ++it)
      printf("%d,", *it);
    printf("]\n");
  }
}*/



} // namespace graph_utils

int main3() {
  vector<vector<int> *> all_subsets;
  GetAllSubsetOfVertices(5, &all_subsets);
  printf("The size of the set is %d\n", all_subsets.size());
  vector<string> v;
  v.push_back("01010");
  v.push_back("10101");
  v.push_back("01000");
  v.push_back("10001");
  v.push_back("01010");
  Graph g(v);
  for (int i = 0; i < all_subsets.size(); ++i) {
    bool valid = IsSubsetSafe(g, *all_subsets[i]);
    printf("%s, [", valid ? "YES" : " NO");
    for (int j = 0; j < all_subsets[i]->size(); ++j)
      printf("%d,", (*all_subsets[i])[j]);
    printf("]\n");
  }

  vector<Graph *> upper_obj;
  graph_utils::CanonicalGraphGenerator generator(0);
  generator.GenerateLowerObjects(g, &upper_obj);
  printf("\nThere are %d lower objects\n", upper_obj.size());
  for (int i = 0; i < upper_obj.size(); ++i) {
    vector<string> matrix;
    upper_obj[i]->GetAdjMatrix(&matrix);
    for (int j = 0; j < matrix.size(); ++j)
      printf("%s\n", matrix[j].c_str());
    printf("\n");
  }

  vector<Graph *> originals;
  generator.FindGraphsFromLowerObject(g, &originals);
  printf("\nThere are %d original objects\n", originals.size());
  for (int i = 0; i < originals.size(); ++i) {
    vector<string> matrix;
    originals[i]->GetAdjMatrix(&matrix);
    for (int j = 0; j < matrix.size(); ++j)
      printf("%s\n", matrix[j].c_str());
    printf("\n");
  }
}

void DeleteVectorOfGraphs(vector<Graph *> *v) {
  while (!v->empty()) {
    delete v->back();
    v->pop_back();
  }
}

int main() {
  vector<Graph *> *cur = new vector<Graph *>();
  vector<Graph *> *next = new vector<Graph *>();

  graph_utils::CanonicalGraphGenerator generator(0);

  Graph *k2 = new Graph(2);
  k2->AddEdge(0, 1);
  cur->push_back(k2);
  const int target_n = 9;

  for (int n = 3; n < target_n; ++n) {
    IsomorphismChecker checker(true);

    for (int graph_index = 0; graph_index < cur->size(); ++graph_index) {
      const Graph &g = *(*cur)[graph_index];
      vector<Graph *> upper_obj;
      generator.GenerateUpperObjects(g, &upper_obj);
      for (int i = 0; i < upper_obj.size(); ++i) {
        vector<Graph *> related_lower_obj;
        generator.GetAllRelatedLowerObjects(*upper_obj[i], &related_lower_obj);
        vector<Graph *> originals;
        for (int lower_index = 0;
            lower_index < related_lower_obj.size() && originals.empty();
            ++lower_index) {
          generator.FindGraphsFromLowerObject(*related_lower_obj[lower_index],
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
    int connected = 0;
    for (int i = 0; i < next->size(); ++i) {
      if ((*next)[i]->IsConnected()) ++connected;
    }
    printf("For n = %d there are in total %d graphs; connected -> %d \n", n, next->size(), connected);
    DeleteVectorOfGraphs(cur);
    delete cur;
    cur = next;
    next = new vector<Graph *>();
  }
}

int main4() {
  IsomorphismChecker checker(true);
  vector<Graph *> next;
  {
    Graph g(3);
    g.AddEdge(0, 1);
    g.AddEdge(1, 2);
    graph_utils::CanonicalGraphGenerator generator(0);

    vector<Graph *> upper_obj;
    generator.GenerateUpperObjects(g, &upper_obj);
    for (int i = 0; i < upper_obj.size(); ++i) {
      vector<string> matrix;
      upper_obj[i]->GetAdjMatrix(&matrix);
      printf("Upper\n");
      for (int j = 0; j < matrix.size(); ++j)
        printf("%s\n", matrix[j].c_str());
      printf("\n");

      vector<Graph *> related_lower_obj;
      generator.GetAllRelatedLowerObjects(*upper_obj[i], &related_lower_obj);
      vector<Graph *> originals;
      for (int lower_index = 0;
          lower_index < related_lower_obj.size() && originals.empty();
          ++lower_index) {
        generator.FindGraphsFromLowerObject(*related_lower_obj[lower_index],
                                            &originals);
      }
      while (!originals.empty()) {
        if (!checker.AddGraphToCheck(originals.back())) {
          delete originals.back();
        }
        originals.pop_back();
      }
    }
  }
  {
    Graph g(3);
    g.AddEdge(0, 1);
    g.AddEdge(0, 2);
    g.AddEdge(1, 2);
    graph_utils::CanonicalGraphGenerator generator(0);

    vector<Graph *> upper_obj;
    generator.GenerateUpperObjects(g, &upper_obj);
    for (int i = 0; i < upper_obj.size(); ++i) {
      vector<string> matrix;
      upper_obj[i]->GetAdjMatrix(&matrix);
      printf("Upper\n");
      for (int j = 0; j < matrix.size(); ++j)
        printf("%s\n", matrix[j].c_str());
      printf("\n");

      vector<Graph *> related_lower_obj;
      generator.GetAllRelatedLowerObjects(*upper_obj[i], &related_lower_obj);
      vector<Graph *> originals;
      for (int lower_index = 0;
          lower_index < related_lower_obj.size() && originals.empty();
          ++lower_index) {
        generator.FindGraphsFromLowerObject(*related_lower_obj[lower_index],
                                            &originals);
      }
      while (!originals.empty()) {
        if (!checker.AddGraphToCheck(originals.back())) {
          delete originals.back();
        }
        originals.pop_back();
      }
    }
  }
  checker.GetAllNonIsomorphicGraphs(&next);
  printf("There are %d graphs constructed\n", next.size());
  int connected = 0;
  for (int i = 0; i < next.size(); ++i) {
    if (!next[i]->IsConnected()) continue;
    ++connected;
    vector<string> matrix;
    next[i]->GetAdjMatrix(&matrix);
    for (int j = 0; j < matrix.size(); ++j)
      printf("%s\n", matrix[j].c_str());
    printf("\n");
  }
  printf("Only %d graphs are connected\n", connected);
}
