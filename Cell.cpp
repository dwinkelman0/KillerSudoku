#include "Cell.h"
#include "Cage.h"

template <uint32_t N> Cell<N>::Cell() : id_(idCounter_++), cages_() {
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
  for (uint32_t i : possibleValues_) {
    bool allCages = true;
    for (LogicalCage<N> *cage : cages_) {
      if (!cage->testCellValues({{this, i + 1}})) {
        allCages = false;
        break;
      }
    }
    newPossibleValues.set(i, allCages);
  }
  if (newPossibleValues != possibleValues_) {
    std::cout << *this << ": ";
    printDifference<N>(std::cout, possibleValues_, newPossibleValues);
    std::cout << std::endl;
    possibleValues_ = newPossibleValues;
    return true;
  }
  return false;
}

template <uint32_t N>
bool Cell<N>::narrowPossibleValues(Cell<N> *other, uint32_t difference) {
  PossibleValues<N> newValuesThis =
      possibleValues_ & (other->possibleValues_ << difference);
  PossibleValues<N> newValuesOther =
      other->possibleValues_ & (possibleValues_ >> difference);
  if (newValuesThis != possibleValues_ ||
      newValuesOther != other->possibleValues_) {
    if (newValuesThis != possibleValues_) {
      std::cout << *this << ": ";
      printDifference<N>(std::cout, possibleValues_, newValuesThis);
      std::cout << std::endl;
      possibleValues_ = newValuesThis;
    }
    if (newValuesOther != other->possibleValues_) {
      std::cout << *this << ": ";
      printDifference<N>(std::cout, other->possibleValues_, newValuesOther);
      std::cout << std::endl;
      other->possibleValues_ = newValuesOther;
    }
    return true;
  }
  return false;
}

template <uint32_t N> uint32_t Cell<N>::idCounter_ = 0;

template class Cell<9>;
