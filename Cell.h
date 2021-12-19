#pragma once

#include <bitset>
#include <iostream>
#include <set>
#include <stdint.h>
#include <stack>

template <uint32_t N> class LogicalCage;

template <uint32_t N> class Cell {
  friend class LogicalCage<N>;
  friend int main();

public:
  Cell();
  void addCage(LogicalCage<N> *cage) {
    for (LogicalCage<N> *other : cages_) {
      other->registerNewOverlappingCage(cage);
    }
    cages_.insert(cage);
  }
  void removeCage(LogicalCage<N> *cage) {
    for (LogicalCage<N> *other : cages_) {
      other->unregisterCage(cage);
    }
  }
  bool operator<(const Cell &other) const { return id_ < other.id_; }
  bool operator=(const Cell &other) const { return id_ == other.id_; }
  std::bitset<N> getPossibleValues() const { return possibleValues_; }
  bool isSolved() const { return possibleValues_.count() == 1; }
  bool isConflict() const {return possibleValues_.count() == 0; }
  uint32_t getNumCages() const { return cages_.size(); }
  void manuallySetValue(const uint32_t value) { possibleValues_.reset(); possibleValues_.set(value);}
  void saveState() { cachedPossibleValues_.push(possibleValues_);}
  void restoreState() { possibleValues_ = cachedPossibleValues_.top(); cachedPossibleValues_.pop();}

  bool narrowPossibleValues();

  /**
   * This cell and another cell are related by a constant difference, i.e. this
   * - other = difference.
   */
  bool narrowPossibleValues(Cell<N> *other, uint32_t difference);

  template <uint32_t N_>
  friend std::ostream &operator<<(std::ostream &os, const Cell<N_> &cell);

private:
  static uint32_t idCounter_;
  uint32_t id_;
  std::set<LogicalCage<N> *>
      cages_;                     /*!< Cages of which this cell is a member. */
  std::bitset<N> possibleValues_; /*!< Values which this call may contain. This
                          is a conservative approximation, so it may contain
                          false values, but the true value is guaranteed to be
                          included. */
                          std::stack<std::bitset<N>> cachedPossibleValues_; /*!< Store state for backtracking. */
};

template <uint32_t N>
std::ostream &operator<<(std::ostream &os, const Cell<N> &cell) {
  os << cell.id_;
  return os;
}
