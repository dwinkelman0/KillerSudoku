#include "Cage.h"
#include "Cell.h"
#include "Util.h"

#include <algorithm>

template <uint32_t N>
Cage<N>::Cage(std::set<Cell<N> *> cells, const uint32_t sum)
    : cells_(cells), sum_(sum) {}

template <uint32_t N>
Cage<N>::Cage(std::set<Cage<N> *> cages) : cells_(), sum_(0) {
  for (const Cage<N> *cage : cages) {
    cells_.insert(cage->cells_.begin(), cage->cells_.end());
    sum_ += cage->sum_;
  }
}

template <uint32_t N> bool Cage<N>::isSubsetOf(const Cage<N> *other) const {
  bool all = true;
  for (Cell<N> *cell : cells_) {
    if (other->cells_.find(cell) == other->cells_.cend()) {
      all = false;
      break;
    }
  }
  return all;
}

template <uint32_t N> bool Cage<N>::isSupersetOf(const Cage<N> *other) const {
  return other->isSubsetOf(this);
}

template <uint32_t N> Cage<N> Cage<N>::operator-(const Cage<N> &other) const {
  if (other.getSum() > getSum() || other.getNumCells() > getNumCells()) {
    return Cage<N>();
  }
  std::set<Cell<N> *> difference(getCells());
  for (Cell<N> *const cell : other.getCells()) {
    difference.erase(cell);
  }
  return Cage<N>(difference, getSum() - other.getSum());
}

template <uint32_t N>
LogicalCage<N>::LogicalCage(std::set<Cell<N> *> cells, const uint32_t sum)
    : Cage<N>(cells, sum), uniqueness_(true) {
  needsEvaluation_.set();
}

template <uint32_t N>
LogicalCage<N>::LogicalCage(std::set<Cage<N> *> cages)
    : Cage<N>(cages), uniqueness_(false) {
  needsEvaluation_.set();
}

template <uint32_t N>
LogicalCage<N>::LogicalCage(const Cage<N> &cage)
    : Cage<N>(cage), uniqueness_(false) {
  needsEvaluation_.set();
}

template <uint32_t N> void LogicalCage<N>::init() {
  for (Cell<N> *cell : this->cells_) {
    overlappingCages_.insert(cell->cages_.begin(), cell->cages_.end());
    cell->addCage(this);
  }
  for (LogicalCage<N> *cage : overlappingCages_) {
    if (cage->isSubsetOf(this)) {
      subsetCages_.insert(cage);
      cage->registerNewSupersetCage(this);
    } else if (cage->isSupersetOf(this)) {
      registerNewSupersetCage(cage);
      cage->subsetCages_.insert(this);
    }
  }
}

template <uint32_t N> void LogicalCage<N>::uninit() {
  for (Cell<N> *cell : this->cells_) {
    cell->removeCage(this);
  }
}

template <uint32_t N>
void LogicalCage<N>::registerNewOverlappingCage(LogicalCage<N> *cage) {
  if (cage == this) {
    return;
  }
  overlappingCages_.insert(cage);
  if (cage->isSubsetOf(this)) {
    subsetCages_.insert(cage);
    cage->registerNewSupersetCage(this);
  } else if (cage->isSupersetOf(this)) {
    registerNewSupersetCage(cage);
    cage->subsetCages_.insert(this);
  }
}

template <uint32_t N>
void LogicalCage<N>::registerNewSupersetCage(LogicalCage<N> *cage) {
  supersetCages_.insert(cage);
  updateUniqueness();
}

template <uint32_t N>
void LogicalCage<N>::unregisterCage(LogicalCage<N> *cage) {
  overlappingCages_.erase(cage);
  subsetCages_.erase(cage);
  supersetCages_.erase(cage);
}

template <uint32_t N> void LogicalCage<N>::updateUniqueness() {
  if (uniqueness_) {
    return;
  }
  bool any = false;
  for (LogicalCage<N> *cage : supersetCages_) {
    if (cage->getUniqueness()) {
      any = true;
      break;
    }
  }
  uniqueness_ = any;
}

template <uint32_t N>
Cage<N> LogicalCage<N>::getMaximumSubset(
    const std::set<LogicalCage<N> *> &group) const {
  std::set<Cage<N> *> cages;
  for (LogicalCage<N> *cage : subsetCages_) {
    if (group.find(cage) != group.end()) {
      cages.insert(cage);
    }
  }
  return Cage<N>(cages);
}

template <uint32_t N>
Cage<N> LogicalCage<N>::getMinimumSuperset(
    const std::set<LogicalCage<N> *> &group) const {
  std::set<Cage<N> *> cages;
  for (LogicalCage<N> *cage : overlappingCages_) {
    if (group.find(cage) != group.end()) {
      cages.insert(cage);
    }
  }
  return Cage<N>(cages);
}

template <uint32_t N>
bool LogicalCage<N>::testCellValues(const Cell<N> *cell, const uint32_t value) {
  if (!needsEvaluation_.test(value)) {
    return true;
  }
  needsEvaluation_.reset(value);
  if (sortedCells_.size() != this->getNumCells()) {
    sortedCells_.clear();
    for (const Cell<N> *cell : this->getCells()) {
      sortedCells_.push_back(cell);
    }
  }
  std::sort(sortedCells_.begin(), sortedCells_.end(),
            Cell<N>::orderByCompleteness);
  return testCellValues({{cell, value + 1}}, sortedCells_);
}

template <uint32_t N>
bool LogicalCage<N>::testCellValues(
    const std::map<const Cell<N> *, uint32_t> &pairs,
    std::vector<const Cell<N> *> &remaining) const {
  const uint32_t bitsetSize = N * N + 1;
  using SumBitset_t = std::bitset<bitsetSize>;
  if (this->getSum() >= bitsetSize) {
    // Too large to evaluate, so err on side of caution for now
    return true;
  }
  uint32_t initialValue = 0;
  SumBitset_t possibleSums(0);
  std::bitset<N> valueBitset(0);
  for (const auto &pair : pairs) {
    initialValue += pair.second;
    valueBitset.set(pair.second - 1);
  }
  possibleSums.set(initialValue, true);
  if (getUniqueness()) {
    if (this->getNumCells() - pairs.size() <= 2) {
      // Can use a bit-twiddling trick to efficiently compute this
      SumBitset_t uniquenessBitsets[N];
      for (const auto &pair : pairs) {
        uniquenessBitsets[pair.second - 1].set(initialValue);
      }
      for (const Cell<N> *other : remaining) {
        SumBitset_t stageSums(0);
        if (pairs.find(other) != pairs.end()) {
          continue;
        }
        bool progress = false;
        for (uint32_t i : other->getPossibleValues()) {
          uniquenessBitsets[i] = (~uniquenessBitsets[i] & possibleSums)
                                 << (i + 1);
          if (uniquenessBitsets[i].any()) {
            progress = true;
          }
          stageSums |= uniquenessBitsets[i];
        }
        if (!progress) {
          return false;
        }
        possibleSums = stageSums;
      }
    } else {
      // Recursively solve
      uint32_t minPossibilities = N + 1;
      bool anyWorks = false;
      if (remaining.size() > 0 && pairs.find(*remaining.begin()) != pairs.end()) {
        remaining.erase(remaining.begin());
      }
      const Cell<N> *bestCell = *remaining.begin();
      remaining.erase(remaining.begin());
      std::map<const Cell<N> *, uint32_t> newPairs(pairs.begin(),
                                                    pairs.end());
      for (uint32_t i : bestCell->getPossibleValues()) {
        if (!valueBitset.test(i)) {
          newPairs.emplace(bestCell, i + 1);
          anyWorks = testCellValues(newPairs, remaining);
          if (anyWorks) {
            break;
          }
          newPairs.erase(bestCell);
        }
      }
      remaining.insert(remaining.begin(), bestCell);
      return anyWorks;
    }
  } else {
    // Cage does not have uniqueness
    for (Cell<N> *other : this->getCells()) {
      SumBitset_t stageSums(0);
      if (pairs.find(other) != pairs.end()) {
        continue;
      }
      for (int i = 0; i < N; ++i) {
        if (getUniqueness() && valueBitset.test(i)) {
          continue;
        }
        if (other->getPossibleValues().test(i)) {
          stageSums |= possibleSums << (i + 1);
        }
      }
      possibleSums = stageSums;
    }
  }
  return possibleSums.test(this->getSum());
}

template <uint32_t N>
bool LogicalCage<N>::orderByComplexity(const LogicalCage<N> *left,
                                       const LogicalCage<N> *right) {
  if (left->getNumCells() != right->getNumCells()) {
    return left->getNumCells() < right->getNumCells();
  }
  int32_t median = ((N + 1) * left->getNumCells()) / 2;
  uint32_t leftDist = abs(static_cast<int32_t>(left->getSum()) - median);
  uint32_t rightDist = abs(static_cast<int32_t>(right->getSum()) - median);
  if (leftDist != rightDist) {
    return leftDist < rightDist;
  }
  return left->getCells() < right->getCells();
}

template class Cage<9>;
template class Cage<16>;
template class LogicalCage<9>;
template class LogicalCage<16>;
