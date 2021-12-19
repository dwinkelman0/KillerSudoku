#include "Cage.h"
#include "Cell.h"
#include "Util.h"

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
  if (other.getSum() > getSum() ||
      other.getCells().size() > getCells().size()) {
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
    : Cage<N>(cells, sum), uniqueness_(true) {}

template <uint32_t N>
LogicalCage<N>::LogicalCage(std::set<Cage<N> *> cages)
    : Cage<N>(cages), uniqueness_(false) {}

template <uint32_t N>
LogicalCage<N>::LogicalCage(const Cage<N> &cage)
    : Cage<N>(cage), uniqueness_(false) {}

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
bool LogicalCage<N>::testCellValues(
    const std::map<const Cell<N> *, uint32_t> pairs) const {
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
    if (this->getCells().size() - pairs.size() <= 2) {
      // Can use a bit-twiddling trick to efficiently compute this
      SumBitset_t uniquenessBitsets[N];
      for (const auto &pair : pairs) {
        uniquenessBitsets[pair.second - 1].set(initialValue);
      }
      for (Cell<N> *other : this->getCells()) {
        SumBitset_t stageSums(0);
        if (pairs.find(other) != pairs.end()) {
          continue;
        }
        bool progress = false;
        for (int i = 0; i < N; ++i) {
          if (other->getPossibleValues().test(i)) {
            uniquenessBitsets[i] = (~uniquenessBitsets[i] & possibleSums)
                                   << (i + 1);
            if (uniquenessBitsets[i].any()) {
              progress = true;
            }
            stageSums |= uniquenessBitsets[i];
          }
        }
        if (!progress) {
          return false;
        }
        possibleSums = stageSums;
      }
    } else {
      // Recursively solve
      uint32_t minPossibilities = N + 1;
      const Cell<N> *bestCell;
      for (const Cell<N> *other : this->getCells()) {
        uint32_t count = other->getPossibleValues().count();
        if (pairs.find(other) == pairs.end() && count < minPossibilities) {
          minPossibilities = count;
          bestCell = other;
        }
      }
      bool anyWorks = false;
      for (int i = 0; i < N; ++i) {
        if (bestCell->getPossibleValues().test(i) && !valueBitset.test(i)) {
          std::map<const Cell<N> *, uint32_t> newPairs(pairs.begin(),
                                                       pairs.end());
          newPairs.emplace(bestCell, i + 1);
          anyWorks = testCellValues(newPairs);
          if (anyWorks) {
            break;
          }
        }
      }
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

template class Cage<9>;
template class LogicalCage<9>;
