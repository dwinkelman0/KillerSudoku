#include "Cell.h"
#include "Cage.h"

#include <algorithm>

template <uint32_t N> Cell<N>::Cell(const uint32_t id) : id_(id), cages_() {
  for (int i = 0; i < N; ++i) {
    possibleValues_.set(i);
  }
}

template <uint32_t N>
void printDifference(std::ostream &os, const PossibleValues<N> &orig,
                     const PossibleValues<N> &diff) {
  os << orig << " -> " << diff;
}

template <uint32_t N> bool Cell<N>::narrowPossibleValues() {
  if (isSolved() || isConflict()) {
    return false;
  }
  PossibleValues<N> newPossibleValues;
  if (sortedCages_.size() != cages_.size()) {
    sortedCages_ = std::vector<LogicalCage<N> *>(cages_.begin(), cages_.end());
  }
  std::sort(sortedCages_.begin(), sortedCages_.end(),
            LogicalCage<N>::orderByComplexity);
  for (uint32_t i : possibleValues_) {
    bool allCages = true;
    for (LogicalCage<N> *cage : sortedCages_) {
      if (!cage->testCellValues(this, i)) {
        allCages = false;
        break;
      }
    }
    newPossibleValues.set(i, allCages);
  }
  return setPossibleValues(newPossibleValues);
}

template <uint32_t N>
bool Cell<N>::narrowPossibleValues(Cell<N> *other, const uint32_t difference) {
  PossibleValues<N> newValuesThis =
      possibleValues_ & (other->possibleValues_ << difference);
  PossibleValues<N> newValuesOther =
      other->possibleValues_ & (possibleValues_ >> difference);
  bool thisResult = setPossibleValues(newValuesThis);
  bool otherResult = other->setPossibleValues(newValuesOther);
  return thisResult || otherResult;
}

template <uint32_t N>
bool Cell<N>::orderByCompleteness(const Cell<N> *left, const Cell<N> *right) {
  if (left->getPossibleValues().count() != right->getPossibleValues().count()) {
    return left->getPossibleValues().count() <
           right->getPossibleValues().count();
  }
  return left->id_ < right->id_;
}

template <uint32_t N>
bool Cell<N>::setPossibleValues(const PossibleValues<N> &values) {
  if (values != possibleValues_) {
    // std::cout << *this << ": ";
    // printDifference<N>(std::cout, possibleValues_, values);
    // std::cout << std::endl;
    possibleValues_ = values;
    for (LogicalCage<N> *cage : cages_) {
      cage->needsEvaluation();
    }
    return true;
  }
  return false;
}

template class Cell<9>;
template class Cell<16>;
