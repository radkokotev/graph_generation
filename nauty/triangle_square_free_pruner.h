
#ifndef TRIANGLE_SQUARE_FREE_PRUNER_H_
#define TRIANGLE_SQUARE_FREE_PRUNER_H_
#define PRUNE

int PRUNE(graph *g,int n,int maxn) {
  if (n < maxn)
    return 0;
  return is_acceptable(g, n);
}

static int is_acceptable(graph *g, int n) {
  const int kMaxGraphSize = 28;
  set *gi;
  int i,j;
  char matrix[kMaxGraphSize][kMaxGraphSize] = { 0 };  // about 800 bytes.

  for (i = 0, gi = (set*)g; i < n; ++i, ++gi)
    for (j = 0; j < n; ++j)
      if (ISELEMENT(gi,j))
        matrix[i][j] = 1;  // put edge

  int count_deg_four = 0;
  int deg_four_vertices[4] = {-1, -1, -1, -1};
  int cur_deg;
  int k, other_deg_four;

  for (i = 0; i < n; ++i) {
    cur_deg = 0;
    for (j = 0; j < n; ++j)
      if (matrix[i][j])
        ++cur_deg;

    if (cur_deg == 4) {
      if (count_deg_four >= 4)
        return 1; // more than 4 vertices with degree 4 ==> PRUNE

      deg_four_vertices[count_deg_four++] = i;
      for (k = 0; k < count_deg_four - 1; ++k) {
        other_deg_four = deg_four_vertices[k]
        if (matrix[i][other_deg_four])
          return 1; // The two deg four vertices are adjacent.

        for (j = 0; j < n; ++j)
          if (matrix[i][j] && matrix[other_deg_four][j])
            return 1;  // Two deg four vertices have a common neighbour.
      }
    }
  }
  return 0;  // No violations found.

}
#endif // TRIANGLE_SQUARE_FREE_PRUNER_H_
