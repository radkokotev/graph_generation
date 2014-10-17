
#include "graph_generator.h"

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <set>
#include <stdexcept>

namespace graph_utils {

SimpleGraphGenerator::SimpleGraphGenerator() {}

bool SimpleGraphGenerator::IsGraphicalDegreeSeq(const std::vector<int> &seq) {
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
    const int vertex, const std::vector<int> &incident_vertices,
    std::vector<int> *seq) {
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
                                  const std::set<int> &forbidden,
                                  const std::vector<int> &seq) {
  if (vertex >= seq.size() || vertex < 0) {
    throw std::invalid_argument("Vertex out of bounds.");
  }
  std::vector<int> leftmost_adj_set;
  const int kAdjSetSize = seq[vertex];
  for (int i = 0; i < seq.size() && leftmost_adj_set.size() < kAdjSetSize;
      ++i) {
    if (forbidden.find(i) != forbidden.end() || i == vertex) {;
      continue;
    }
    leftmost_adj_set.push_back(i);
  }
  if (leftmost_adj_set.size() < kAdjSetSize) {
    // There were not enough edges to connect.
    return false;
  }
  std::vector<int> reduced_seq(seq);
  ReduceDegreeSequence(vertex, leftmost_adj_set, &reduced_seq);
  std::sort(reduced_seq.begin(), reduced_seq.end(), std::greater<int>());
  return IsGraphicalDegreeSeq(reduced_seq);
}

} // namespace graph_utils
