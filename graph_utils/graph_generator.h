// A simple graph generator to enumarate all graphs with a given degree
// sequence. Based on Kim H.,  Toroczkai z., Erdos P., Miklos I., Szekely L.,
// "Degree-based graph construction" http://arxiv.org/pdf/0905.4892

#ifndef GRAPH_UTILS_GRAPH_GENERATOR_H_
#define GRAPH_UTILS_GRAPH_GENERATOR_H_

#include <set>
#include <utility>
#include <vector>

#include "graph.h"
#include "graph_utilities.h"

namespace graph_utils {

class SimpleGraphGenerator {
public:
  SimpleGraphGenerator();

  // Erdos-Gallai Theorem
  static bool IsGraphicalDegreeSeq(const std::vector<int> &seq);

  // Returns true if the constrained graphicality test holds for the given
  // vertex. 'seq_allowed' is a degree sequence in non-increasing order and
  // every boolean value represents if the vertex is allowed or restricted.
  static bool CGTest(const int vertex,
                     const std::vector<std::pair<int, bool> > &seq_allowed);

  // Reduces the degree sequence by removing 'vertex' from the graph and all
  // edges that are incident on that vertex.
  static void ReduceDegreeSequence(const int vertex,
                                   const std::vector<int> &incident_vertices,
                                   std::vector<int> *seq);

  // Generates all valid adjacency sets for vertex 0. Expects a reverse sorted
  // degree sequence, 'cur_set' needs to be empty initially and is altered.
  // The resulting adjacency sets are appended to 'adj_sets'.
  static void GenerateAllAdjSets(const std::vector<int> &original_seq,
                                 std::set<int> *cur_set,
                                 std::vector<std::set<int> > *adj_sets);

  // Generates all graphs with the given degree sequence. No isomorphic
  // elimination is applied, generated graphs are not guaranteed to be
  // connected.
  static void GenerateAllGraphs(const std::vector<int> &seq,
                                std::vector<Graph *> *graphs);

  // Generates all unique connected graphs with the given degree sequence.
  static void GenerateAllUniqueGraphs(const std::vector<int> &seq,
                                      GraphFilter *filter,
                                      std::vector<Graph *> *graphs);

  // Generates all non-increasing degree sequences for n vertices. The generated
  // sequences are not guaranteed to be graphical. Graphicality needs to be
  // verified separately.
  static void GenerateAllDegreeSequences(const int n,
                                         std::vector<std::vector<int> > *seqs);

private:
  // A helper function to construct all graphs for the given degree sequence
  // following the algoruithm described in the article above. If
  // 'unique_graphs_only' is false all graphs are generated without pruning.
  // If it is set to true, the filter is used to prune the search, only
  // connected graphs are added and isomorphic copies are eliminated.
  static void GenerateAllGraphs(const std::vector<std::pair<int, int> > &seq,
                                const bool unique_graphs_only,
                                GraphFilter *filter, Graph *g,
                                std::vector<Graph *> *graphs);

  // A helper function to recursively generate all non-increasing degree
  // sequences of order n.
  static void GenerateAllDegreeSequences(const int n, std::vector<int> *cur_seq,
                                         std::vector<std::vector<int> > *seqs);
};

} // namespace graph_utils

#endif // GRAPH_UTILS_GRAPH_GENERATOR_H_
