#include "Puzzle.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

static std::vector<uint32_t> randomRange(const uint32_t n) {
  std::vector<uint32_t> output;
  for (uint32_t i = 0; i < n; ++i) {
    output.push_back(i);
  }
  std::random_shuffle(output.begin(), output.end());
  return output;
}

template <uint32_t R, uint32_t C> Puzzle<R, C>::Solution::Solution() {
  // Generate first row
  // Generate canonical solution
  for (uint32_t row = 0; row < N; ++row) {
    uint32_t subrow = row % R;
    uint32_t band = row / R;
    uint32_t startingValue = subrow * C + band;
    for (uint32_t col = 0; col < N; ++col) {
      data_[row * N + col] = (startingValue + col) % N;
    }
  }

  // // Choose numbers to rotate-swap
  // std::set<uint32_t> rowSwapSet;
  // for (uint32_t i = 0; i < R; ++i) {
  //   rowSwapSet.insert(*randomRange(C).begin() + i * C);
  // }
  // for (uint32_t row = 0; row < N; ++row) {
  //   std::set<uint32_t> rowSwapSetCopy(rowSwapSet);
  //   for (uint32_t col = 0; col < C; ++col) {
  //     rowSwapSetCopy.erase(data_[row * N + col]);
  //   }
  //   std::vector<uint32_t> swapVector(rowSwapSetCopy.begin(),
  //   rowSwapSetCopy.end()); std::map<uint32_t, uint32_t> swapMapping; for
  //   (uint32_t i = 0; i < swapVector.size(); ++i) {
  //     swapMapping.emplace(swapVector[i], swapVector[(i + 1) %
  //     swapVector.size()]);
  //   }
  //   for (uint32_t col = C; col < N; ++col) {
  //     auto it = swapMapping.find(data_[row * N + col]);
  //     if (it != swapMapping.end()) {
  //       data_[row * N + col] = it->second;
  //     }
  //   }
  // }
  // std::set<uint32_t> colSwapSet;
  // for (uint32_t i = 0; i < C; ++i) {
  //   colSwapSet.insert(*randomRange(R).begin() + i * R);
  // }
  // for (uint32_t col = 0; col < N; ++col) {
  //   std::set<uint32_t> colSwapSetCopy(colSwapSet);
  //   for (uint32_t row = 0; row < R; ++row) {
  //     colSwapSetCopy.erase(data_[row * N + col]);
  //   }
  //   std::vector<uint32_t> swapVector(colSwapSetCopy.begin(),
  //   colSwapSetCopy.end()); std::map<uint32_t, uint32_t> swapMapping; for
  //   (uint32_t i = 0; i < swapVector.size(); ++i) {
  //     swapMapping.emplace(swapVector[i], swapVector[(i + 1) %
  //     swapVector.size()]);
  //   }
  //   for (uint32_t row = R; row < N; ++row) {
  //     auto it = swapMapping.find(data_[row * N + col]);
  //     if (it != swapMapping.end()) {
  //       data_[row * N + col] = it->second;
  //     }
  //   }
  // }

  // Randomly mix bands, stacks, rows, and columns
  uint32_t newData[N * N];
  std::vector<uint32_t> bandOrder = randomRange(C);
  std::vector<uint32_t> stackOrder = randomRange(R);
  std::vector<std::vector<uint32_t>> rowOrders, colOrders;
  for (uint32_t i = 0; i < C; ++i) {
    rowOrders.push_back(randomRange(R));
  }
  for (uint32_t i = 0; i < R; ++i) {
    colOrders.push_back(randomRange(C));
  }
  for (uint32_t row = 0; row < N; ++row) {
    uint32_t band = row / R;
    uint32_t subrow = row % R;
    for (uint32_t col = 0; col < N; ++col) {
      uint32_t stack = col / C;
      uint32_t subcol = col % C;
      newData[row * N + col] =
          get(bandOrder[band] * R + rowOrders[band][subrow],
              stackOrder[stack] * C + colOrders[stack][subcol]);
    }
  }

  // Relabel numbers
  std::vector<uint32_t> relabelMap = randomRange(N);
  for (uint32_t i = 0; i < N * N; ++i) {
    data_[i] = relabelMap[newData[i]] + 1;
  }
}

template class Puzzle<4, 2>;
template class Puzzle<3, 3>;
template class Puzzle<4, 4>;
