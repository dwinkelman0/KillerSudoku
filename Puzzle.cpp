#include "Puzzle.h"

#include "Cage.h"
#include "Solver.h"

#include <algorithm>
#include <random>
#include <set>
#include <vector>

template <uint32_t R, uint32_t C>
Puzzle<R, C>::CellMap::CellMap(const std::vector<Cage<N> *> &cages)
    : activeCages_(cages.begin(), cages.end()) {
  for (Cage<N> *cage : cages) {
    for (Cell<N> *cell : cage->getCells()) {
      cageMap_.emplace(cell, cage);
    }
  }
}

template <uint32_t R, uint32_t C>
bool Puzzle<R, C>::CellMap::merge(Cell<N> *cell1, Cell<N> *cell2,
                                  const uint32_t maxCageSize) {
  if (!areInSameCage(cell1, cell2) &&
      cageMap_[cell1]->getNumCells() + cageMap_[cell2]->getNumCells() <=
          maxCageSize) {
    Cage<N> *newCage = new Cage<N>({cageMap_[cell1], cageMap_[cell2]});
    activeCages_.insert(newCage);
    history_.push({cageMap_[cell1], cageMap_[cell2]});
    for (Cell<N> *cell : newCage->getCells()) {
      auto it = activeCages_.find(cageMap_[cell]);
      if (it != activeCages_.end()) {
        activeCages_.erase(it);
      }
      cageMap_[cell] = newCage;
    }
    return true;
  }
  return false;
}

template <uint32_t R, uint32_t C> void Puzzle<R, C>::CellMap::unmerge() {
  if (history_.size() == 0) {
    return;
  }
  activeCages_.erase(cageMap_[*history_.top().first->getCells().begin()]);
  restoreCage(history_.top().first);
  restoreCage(history_.top().second);
  activeCages_.insert(history_.top().first);
  activeCages_.insert(history_.top().second);
  history_.pop();
}

template <uint32_t R, uint32_t C>
bool Puzzle<R, C>::CellMap::areInSameCage(Cell<N> *cell1,
                                          Cell<N> *cell2) const {
  return getCage(cell1) == getCage(cell2);
}

template <uint32_t R, uint32_t C>
Cage<Puzzle<R, C>::N> *Puzzle<R, C>::CellMap::getCage(Cell<N> *cell) const {
  auto it = cageMap_.find(cell);
  return it == cageMap_.end() ? nullptr : it->second;
}

template <uint32_t R, uint32_t C>
std::set<LogicalCage<Puzzle<R, C>::N> *>
Puzzle<R, C>::CellMap::generateLogicalCages() const {
  std::set<LogicalCage<Puzzle<R, C>::N> *> output;
  for (Cage<N> *cage : activeCages_) {
    LogicalCage<N> *newCage = new LogicalCage<N>(*cage);
    output.insert(newCage);
  }
  return output;
}

template <uint32_t R, uint32_t C>
void Puzzle<R, C>::CellMap::restoreCage(Cage<N> *cage) {
  for (Cell<N> *cell : cage->getCells()) {
    cageMap_[cell] = cage;
  }
}

template <uint32_t R, uint32_t C> Puzzle<R, C>::Puzzle() {
  std::random_device randomDevice;
  std::mt19937 generator(randomDevice());
  std::uniform_int_distribution<uint32_t> orientationSelector(0, 1);
  std::uniform_int_distribution<uint32_t> rowSelector(0, N - 1);
  std::uniform_int_distribution<uint32_t> colSelector(0, N - 2);

  Solution solution;
  std::vector<Cage<N> *> initialCages;
  for (uint32_t row = 0; row < N; ++row) {
    for (uint32_t col = 0; col < N; ++col) {
      Cell<N> *newCell = new Cell<N>(row * N + col);
      cells_.push_back(newCell);
      initialCages.push_back(new Cage<N>({newCell}, solution.get(row, col)));
    }
  }
  CellMap cellMap(initialCages);
  for (uint32_t cageSize = 2; cageSize < 7 && cellMap.getNumCages() > N * N / 3; ++cageSize) {
    uint32_t numCages = N * N * 2 / (cageSize + 1);
    while (cellMap.getNumCages() > numCages) {
      bool orientation = orientationSelector(generator);
      uint32_t row = rowSelector(generator);
      uint32_t col = colSelector(generator);
      uint32_t i1 = orientation ? row * N + col : col * N + row;
      uint32_t i2 = orientation ? row * N + col + 1 : (col + 1) * N + row;
      if (cellMap.merge(cells_[i1], cells_[i2], cageSize)) {
        std::cout << "Merge " << *cells_[i1] << " and " << *cells_[i2] << " ("
                  << cellMap.getNumCages() << " cages)" << std::endl;
        auto definedCages = cellMap.generateLogicalCages();
        uint32_t numSolutions =
            solve(cells_, definedCages, SolverMode::DUPLICATE_SOLUTIONS);
        if (numSolutions > 1) {
          std::cout << "No unique solution, unmerge" << std::endl;
          cellMap.unmerge();
        }
      }
    }
  }
  for (Cell<N> *cell : cells_) {
    std::cout << *cell << ": " << *cellMap.getCage(cell) << std::endl;
  }
}

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

template class Puzzle<3, 3>;
