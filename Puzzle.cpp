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

template <uint32_t R, uint32_t C> Puzzle<R, C>::Puzzle(const uint32_t numCages) {
  std::random_device randomDevice;
  std::mt19937 generator(randomDevice());
  std::uniform_int_distribution<uint32_t> orientationSelector(0, 1);
  std::uniform_int_distribution<uint32_t> rowSelector(0, N - 1);
  std::uniform_int_distribution<uint32_t> colSelector(0, N - 2);

  Solution solution = Solution()
                          .swapOrder()
                          .swapValues(true)
                          .swapOrder()
                          .swapValues(false)
                          .swapOrder();
  std::cout << "Solution:" << std::endl;
  for (uint32_t row = 0; row < N; ++row) {
    for (uint32_t col = 0; col < N; ++col) {
      std::cout << solution.get(row, col) << ", ";
    }
    std::cout << std::endl;
  }
  std::vector<Cage<N> *> initialCages;
  for (uint32_t row = 0; row < N; ++row) {
    for (uint32_t col = 0; col < N; ++col) {
      Cell<N> *newCell = new Cell<N>(row * N + col);
      cells_.push_back(newCell);
      initialCages.push_back(
          new Cage<N>({newCell}, solution.get(row, col) + 1));
    }
  }
  CellMap cellMap(initialCages);
  for (uint32_t cageSize = 2; cageSize < 8 && cellMap.getNumCages() > numCages;
       ++cageSize) {
    uint32_t numStageCages = N * N * 2 / (cageSize + 1);
    uint32_t attempts = 0;
    while (cellMap.getNumCages() > numStageCages && attempts < 10) {
      bool orientation = orientationSelector(generator);
      uint32_t row = rowSelector(generator);
      uint32_t col = colSelector(generator);
      uint32_t i1 = orientation ? row * N + col : col * N + row;
      uint32_t i2 = orientation ? row * N + col + 1 : (col + 1) * N + row;
      if (cellMap.merge(cells_[i1], cells_[i2], cageSize)) {
        std::cout << "Merge " << *cells_[i1] << " and " << *cells_[i2] << " ("
                  << cellMap.getNumCages() << " cages)" << std::endl;
        std::set<uint32_t> cageValues;
        for (Cell<N> *cell : cellMap.getCage(cells_[i1])->getCells()) {
          cageValues.insert(solution.get(cell->getId()));
        }
        if (cageValues.size() != cellMap.getCage(cells_[i1])->getNumCells()) {
          // Cage uniqueness constraint violated
          std::cout << "Cage does not have unique values, unmerge" << std::endl;
          cellMap.unmerge();
          continue;
        }
        auto definedCages = cellMap.generateLogicalCages();
        uint32_t numSolutions =
            solve(cells_, definedCages, SolverMode::DUPLICATE_SOLUTIONS);
        if (numSolutions > 1) {
          std::cout << "No unique solution, unmerge" << std::endl;
          cellMap.unmerge();
          ++attempts;
        } else {
          attempts = 0;
        }
      }
    }
    if (attempts == 10) {
      std::cout << numCages << " cages was too damn hard, this puzzle has " << cellMap.getNumCages() << " cages instead" << std::endl;
    }
  }
  auto definedCages = cellMap.generateLogicalCages();
  std::cout << "const cages = [" << std::endl;
  for (Cage<N> *cage : definedCages) {
    std::cout << "\t{cells: [";
    for (Cell<N> *cell : cage->getCells()) {
      std::cout << *cell << ", ";
    }
    std::cout << "], sum: " << cage->getSum() << "}," << std::endl;
  }
  std::cout << "];" << std::endl;
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
}

template <uint32_t R, uint32_t C>
typename Puzzle<R, C>::Solution
Puzzle<R, C>::Solution::swapValues(const bool axis) const {
  Solution output = *this;
  std::vector<uint32_t> swapOrder = randomRange(N);
  std::set<uint32_t> swapSet(swapOrder.begin(), swapOrder.begin() + N / 2);
  std::vector<Cell<N> *> cells;
  std::set<LogicalCage<N> *> definedCages;
  for (uint32_t row = 0; row < N; ++row) {
    for (uint32_t col = 0; col < N; ++col) {
      Cell<N> *newCell = new Cell<N>(row * N + col);
      cells.push_back(newCell);
      if (swapSet.find(get(row, col)) == swapSet.end() || (col < C && axis) ||
          (row < R && !axis)) {
        definedCages.insert(new LogicalCage<N>({newCell}, get(row, col) + 1));
      }
    }
  }
  solve(cells, definedCages, SolverMode::ANY_SOLUTIONS);
  for (Cell<N> *cell : cells) {
    output.data_[cell->getId()] = *cell->getPossibleValues().begin();
  }
  return output;
}

template <uint32_t R, uint32_t C>
typename Puzzle<R, C>::Solution Puzzle<R, C>::Solution::swapOrder() const {
  Solution output;

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
    output.data_[i] = relabelMap[newData[i]];
  }
  return output;
}

template class Puzzle<3, 3>;
