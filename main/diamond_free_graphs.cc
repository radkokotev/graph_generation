// A main program to generate all diamond free graphs with N vertices.

#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <ctime>

#include "graph_utils/graph.h"
#include "graph_utils/graph_utilities.h"
#include "graph_utils/graph_generator.h"
#include "nauty_utils/nauty_wrapper.h"

using std::vector;
using std::queue;
using graph_utils::SimpleGraphGenerator;
using graph_utils::Graph;
using graph_utils::DiamondFreeGraph;

namespace {

const int kNumberOfThreads = 4;
const int kNumberOfVertices = 7;
int worker_threads_alive = 0;
std::mutex mutex;
std::mutex mutex_isomorphism;
std::condition_variable cond_var;
vector<vector<int>> seqs;
queue<Graph> q;

int64_t final_count = 0;
int64_t all_graphs_count = 0;
int64_t all_connected_diamond_free_count = 0;
bool done_generating = false;

void EliminateIsomorphs() {
  std::unique_lock<std::mutex> lck(mutex_isomorphism, std::defer_lock);
  nauty_utils::IsomorphismChecker checker(true);
  while (true) {
    lck.lock();
    while (q.empty() && !done_generating) {
      cond_var.wait(lck);
    }
    if (!q.empty()) {
      Graph *g = new Graph(q.front());
      q.pop();
      lck.unlock();
      if (!checker.AddGraphToCheck(g)) {
        delete g;
      }
    } else {
      if (q.empty() && done_generating) {
        lck.unlock();
        vector<Graph *> non_isomorphic;
        checker.GetAllNonIsomorphicGraphs(&non_isomorphic);
        final_count = non_isomorphic.size();
        return;
      }
      lck.unlock();
    }
  }
}

void GenerateAllGraphsWithNextSeq() {
  std::unique_lock<std::mutex> lck(mutex_isomorphism, std::defer_lock);
  while (true) {
    mutex.lock();
    if (seqs.empty()) {
      --worker_threads_alive;
      done_generating = worker_threads_alive <= 0;
      printf("alive = %d\n", worker_threads_alive);
      mutex.unlock();
      lck.lock();
      cond_var.notify_all();
      lck.unlock();
      return;
    }
    vector<int> seq = seqs.back();
    seqs.pop_back();
    mutex.unlock();

    vector<Graph> all_graphs;
    SimpleGraphGenerator::GenerateAllGraphs(seq, &all_graphs);
    int count_diamond_free = 0;


    for (int i = 0; i < all_graphs.size(); ++i) {
      if (all_graphs[i].IsConnected() &&
          DiamondFreeGraph::IsDiamondFree(all_graphs[i])) {
        lck.lock();
        ++count_diamond_free;
        q.push(all_graphs[i]);
        cond_var.notify_all();
        lck.unlock();

      }
    }
    // Update counters.
    mutex.lock();
    all_graphs_count += all_graphs.size();
    all_connected_diamond_free_count += count_diamond_free;
    mutex.unlock();
  }
}

} // namespace

int main() {
  std::clock_t start = std::clock();
  SimpleGraphGenerator::GenerateAllDegreeSequences(kNumberOfVertices, &seqs);
  std::thread threads[kNumberOfThreads];
  worker_threads_alive = kNumberOfThreads - 1;
  for (int i = 0; i < kNumberOfThreads - 1; ++i) {
    threads[i] = std::thread(GenerateAllGraphsWithNextSeq);
  }
  threads[kNumberOfThreads - 1] = std::thread(EliminateIsomorphs);
  for (int i = 0; i < kNumberOfThreads; ++i) {
    threads[i].join();
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
