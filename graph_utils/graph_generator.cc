// Implementation of SimpleGraphGenerator
#include "graph_generator.h"

#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <set>
#include <stdexcept>
#include <utility>

#include "graph.h"

using std::vector;
using std::set;
using std::pair;
using std::make_pair;

namespace graph_utils {

SimpleGraphGenerator::SimpleGraphGenerator() {}

bool SimpleGraphGenerator::IsGraphicalDegreeSeq(const vector<int> &seq) {
  int sum = 0;
  for (int i = 0; i < seq.size(); ++i) {
    sum += seq[i];
    if (i < seq.size() - 1 && seq[i] < seq[i + 1]) {
      throw std::invalid_argument(
          "The degree sequence must be in non-increasing order.");
    }
  }
  if (sum % 2) {
    return false;
  }

  for (int k = 1; k <= seq.size(); ++k) {
    int left_sum = 0;
    int right_sum = k * (k - 1);
    for (int i = 1; i <= k; ++i) {
      left_sum += seq[i - 1]; // 0-based index
    }
    for (int i = k + 1; i <= seq.size(); ++i) {
      right_sum += std::min(k, seq[i - 1]); // 0-based index
    }
    if (left_sum > right_sum) {
      return false;
    }
    if (k < seq.size() && seq[k - 1] >= k && seq[k] < k + 1) {
      break;
    }
  }
  return true;
}

void SimpleGraphGenerator::ReduceDegreeSequence(
    const int vertex, const vector<int> &incident_vertices,
    vector<int> *seq) {
  if (vertex >= seq->size() || vertex < 0) {
    throw std::invalid_argument("Vertex out of bounds.");
  }
  if ((*seq)[vertex] != incident_vertices.size()) {
    throw std::invalid_argument("Mismatching number of incident vertices.");
  }
  (*seq)[vertex] = 0;
  for (int i = 0; i < incident_vertices.size(); ++i) {
    if (incident_vertices[i] >= seq->size() || incident_vertices[i] < 0) {
      throw std::invalid_argument("Incident vertex out of bounds.");
    }
    (*seq)[incident_vertices[i]]--;
    if ((*seq)[incident_vertices[i]] < 0) {
      throw std::invalid_argument("Invalid incident vertex.");
    }
  }
}

bool SimpleGraphGenerator::CGTest(const int vertex,
                                  const vector<pair<int, bool>> &seq_allowed) {
  if (vertex >= seq_allowed.size() || vertex < 0) {
    throw std::invalid_argument("Vertex out of bounds.");
  }
  vector<int> leftmost_adj_set;
  const int kAdjSetSize = seq_allowed[vertex].first;
  for (int i = 0; i < seq_allowed.size() && leftmost_adj_set.size() < kAdjSetSize;
      ++i) {
    if (!seq_allowed[i].second || i == vertex) {;
      continue;
    }
    leftmost_adj_set.push_back(i);
  }
  if (leftmost_adj_set.size() < kAdjSetSize) {
    // There were not enough edges to connect.
    return false;
  }
  vector<int> reduced_seq(seq_allowed.size(), 0);
  for (int i = 0; i < seq_allowed.size(); ++i) {
    reduced_seq[i] = seq_allowed[i].first;
  }
  ReduceDegreeSequence(vertex, leftmost_adj_set, &reduced_seq);
  std::sort(reduced_seq.begin(), reduced_seq.end(), std::greater<int>());
  return IsGraphicalDegreeSeq(reduced_seq);
}

void SimpleGraphGenerator::GenerateAllAdjSets(const vector<int> &original_seq,
                                              set<int> *cur_set,
                                              vector<set<int>> *adj_sets) {
  if (cur_set->size() >= original_seq[0]) {
    // This set is complete.
    adj_sets->push_back(*cur_set);
    return;
  }
  int vertex = original_seq.size() - 1;
  if (!cur_set->empty()) {
    vertex = (*(cur_set->begin())) - 1;
  }

  if (vertex <= 0) {
    // Invalid adjacency set.
    return;
  }

  for (; vertex > 0; --vertex) {
    cur_set->insert(vertex);  // Temporarily inset the vertex

    int new_vertex_degree = original_seq[0] - cur_set->size();

    // Create the sequence reduced by the newly introduced edge.
    vector<pair<int, bool>> new_seq;

    new_seq.push_back(make_pair(new_vertex_degree, false));
    for (int i = 1; i < original_seq.size(); ++i) {
      new_seq.push_back(make_pair(original_seq[i], true));
    }
    for (auto it = cur_set->cbegin(); it != cur_set->cend(); ++it) {
      // All edges in the set are forbidden for new connections.
      --new_seq[*it].first;
      new_seq[*it].second = false;
    }

    std::sort(new_seq.rbegin(), new_seq.rend());  // reverse sort

    int new_vertex_index = original_seq.size() - 1 -
        (std::lower_bound(new_seq.rbegin(), new_seq.rend(),
                         make_pair(new_vertex_degree, false)) -
            new_seq.rbegin());

    if (CGTest(new_vertex_index, new_seq)) {
      GenerateAllAdjSets(original_seq, cur_set, adj_sets);
    }
    cur_set->erase(vertex);  // Remove the vertex, which was temporarily added.
  }
}

void SimpleGraphGenerator::GenerateAllGraphs(const vector<int> &seq,
                                             vector<Graph> *graphs) {
  Graph g(seq.size());
  vector<pair<int, int>> new_seq;
  for (int i = 0; i < seq.size(); ++i) {
    new_seq.push_back(make_pair(seq[i], i));
  }
  GenerateAllGraphs(new_seq, &g, graphs);
}

void SimpleGraphGenerator::GenerateAllGraphs(
    const vector<pair<int, int>> &seq,  // [ (deg, vertex), (deg, vertex), ...]
    Graph *g,
    vector<Graph> *graphs) {
  if (seq.front().first <= 0) {
    graphs->push_back(*g);
    return;
  }
  vector<int> helper_seq;
  for (int i = 0; i < seq.size(); ++i) {
    if (seq[i].first <= 0) {
      break;
    }
    helper_seq.push_back(seq[i].first);
  }
  set<int> helper_set;
  vector<set<int>> adj_sets;
  GenerateAllAdjSets(helper_seq, &helper_set, &adj_sets);
  for (int i = 0; i < adj_sets.size(); ++i) {
    const set<int> &curr_set = adj_sets[i];

    vector<pair<int, int>> new_seq(seq);
    new_seq[0].first = 0;
    for (auto it = curr_set.cbegin(); it != curr_set.cend(); ++it) {
      g->AddEdge(seq[0].second, seq[*it].second);  // Add temporary edges
      --new_seq[*it].first;  // reduce degree sequence
    }

    std::sort(new_seq.rbegin(), new_seq.rend());  // reverse sort
    GenerateAllGraphs(new_seq, g, graphs);

    for (auto it = curr_set.cbegin(); it != curr_set.cend(); ++it) {
      g->RemoveEdge(seq[0].second, seq[*it].second);  // remove temp edges
    }
  }
}

void SimpleGraphGenerator::GenerateAllDegreeSequences(
    const int n, vector<vector<int>> *seqs) {
  vector<int> work_seq;
  SimpleGraphGenerator::GenerateAllDegreeSequences(n, &work_seq, seqs);
}

void SimpleGraphGenerator::GenerateAllDegreeSequences(
    const int n, vector<int> *cur_seq, vector<vector<int>> *seqs) {
  if (cur_seq->size() >= n) {
    seqs->push_back(*cur_seq);
    return;
  }
  int m = n - 1;
  if (!cur_seq->empty()) {
    m = cur_seq->back();
  }

  cur_seq->push_back(0);
  for (int i = m; i > 0; --i) {
    (*cur_seq)[cur_seq->size() - 1] = i;
    GenerateAllDegreeSequences(n, cur_seq, seqs);
  }
  cur_seq->erase(cur_seq->begin() + cur_seq->size() - 1);
}

} // namespace graph_utils
