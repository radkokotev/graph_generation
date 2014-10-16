
#include "graph_generator.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>
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

} // namespace graph_utils
