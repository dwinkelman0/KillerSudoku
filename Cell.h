#pragma once

#include "PossibleValues.h"

#include <iostream>
#include <set>
#include <stack>
#include <stdint.h>
#include <vector>

template <uint32_t N> class LogicalCage;

template <uint32_t N> class Cell {
  friend class LogicalCage<N>;
  friend int main();

public:
  Cell();
  inline void addCage(LogicalCage<N> *cage) {
    for (LogicalCage<N> *other : cages_) {
      other->registerNewOverlappingCage(cage);
    }
    cages_.insert(cage);
  }
  inline void removeCage(LogicalCage<N> *cage) {
    for (LogicalCage<N> *other : cages_) {
      other->unregisterCage(cage);
    }
  }
  inline bool operator<(const Cell &other) const { return id_ < other.id_; }
  inline bool operator=(const Cell &other) const { return id_ == other.id_; }
  inline PossibleValues<N> getPossibleValues() const { return possibleValues_; }
  inline bool isSolved() const { return possibleValues_.count() == 1; }
  inline bool isConflict() const { return possibleValues_.count() == 0; }
  inline uint32_t getNumCages() const { return cages_.size(); }
  inline void manuallySetValue(const uint32_t value) {
    possibleValues_.reset();
    possibleValues_.set(value);
  }
  inline void saveState() { cachedPossibleValues_.push(possibleValues_); }
  inline void restoreState() {
    possibleValues_ = cachedPossibleValues_.top();
    cachedPossibleValues_.pop();
  }

  bool narrowPossibleValues();

  /**
   * This cell and another cell are related by a constant difference, i.e. this
   * - other = difference.
   */
  bool narrowPossibleValues(Cell<N> *other, uint32_t difference);

  template <uint32_t N_>
  friend std::ostream &operator<<(std::ostream &os, const Cell<N_> &cell);

  static bool orderByCompleteness(const Cell<N> *left, const Cell<N> *right);

private:
  /**
   * Returns true if there was an update to possible values.
   */
  bool setPossibleValues(const PossibleValues<N> &values);

private:
  static uint32_t idCounter_;
  uint32_t id_;
  std::set<LogicalCage<N> *>
      cages_; /*!< Cages of which this cell is a member. */
  PossibleValues<N> possibleValues_; /*!< Values which this call may contain.
                          This is a conservative approximation, so it may
                          contain false values, but the true value is guaranteed
                          to be included. */
  std::stack<PossibleValues<N>>
      cachedPossibleValues_; /*!< Store state for backtracking. */
  std::vector<LogicalCage<N> *> sortedCages_; /*!< Cache for cages ordered by
                                                 how hard they are to solve. */
};

template <uint32_t N>
std::ostream &operator<<(std::ostream &os, const Cell<N> &cell) {
  os << cell.id_;
  return os;
}
