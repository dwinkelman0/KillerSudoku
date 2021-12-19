#include "Cell.h"
#include "Cage.h"

template <uint32_t N> Cell<N>::Cell() : id_(idCounter_++), cages_() {
  for (int i = 0; i < N; ++i) {
    possibleValues_.set(i);
  }
}

template <uint32_t N>
static void printBitset(std::ostream &os, const std::bitset<N> bits) {
  for (int i = 0; i < N; ++i) {
    if (bits.test(i)) {
      os << i + 1 << ", ";
    }
  }
}

template <uint32_t N>
void printDifference(const std::bitset<N> orig, const std::bitset<N> diff) {
  std::cout << "{";
  for (int i = 0; i < N; ++i) {
    if (orig.test(i)) {
      std::cout << i + 1 << ", ";
    }
  }
  std::cout << "} -> {";
  for (int i = 0; i < N; ++i) {
    if (diff.test(i)) {
      std::cout << i + 1 << ", ";
    }
  }
  std::cout << "}";
}

template <uint32_t N> bool Cell<N>::narrowPossibleValues() {
  if (possibleValues_.count() == 1) {
    return false;
  }
  std::bitset<N> newPossibleValues(0);
  for (int i = 0; i < N; ++i) {
    if (possibleValues_.test(i)) {
      bool allCages = true;
      for (LogicalCage<N> *cage : cages_) {
        if (!cage->testCellValues({{this, i + 1}})) {
          allCages = false;
          break;
        }
      }
      newPossibleValues.set(i, allCages);
    }
  }
  if (newPossibleValues != possibleValues_) {
    std::cout << *this << ": ";
    printDifference<N>(possibleValues_, newPossibleValues);
    std::cout << std::endl;
    possibleValues_ = newPossibleValues;
    return true;
  }
  return false;
}

template <uint32_t N>
bool Cell<N>::narrowPossibleValues(Cell<N> *other, uint32_t difference) {
  std::bitset<N> newValuesThis =
      possibleValues_ & (other->possibleValues_ << difference);
  std::bitset<N> newValuesOther =
      other->possibleValues_ & (possibleValues_ >> difference);
  if (newValuesThis != possibleValues_ ||
      newValuesOther != other->possibleValues_) {
    if (newValuesThis != possibleValues_) {
      std::cout << *this << ": ";
      printDifference<N>(possibleValues_, newValuesThis);
      std::cout << std::endl;
      possibleValues_ = newValuesThis;
    }
    if (newValuesOther != other->possibleValues_) {
      std::cout << *this << ": ";
      printDifference<N>(other->possibleValues_, newValuesOther);
      std::cout << std::endl;
      other->possibleValues_ = newValuesOther;
    }
    return true;
  }
  return false;
}

template <uint32_t N> uint32_t Cell<N>::idCounter_ = 0;

template class Cell<9>;
