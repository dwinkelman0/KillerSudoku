#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdint.h>

template <uint32_t N> class Cell;
template <uint32_t N> class Cage;

template <uint32_t N> class Cage {

public:
  Cage() : Cage({}, 0) {}
  Cage(std::set<Cell<N> *> cells, const uint32_t sum);
  Cage(std::set<Cage<N> *> cages);

  inline std::set<Cell<N> *> getCells() const { return cells_; }
  inline uint32_t getNumCells() const { return cells_.size(); }
  inline uint32_t getSum() const { return sum_; }

  /**
   * Check whether this cage is a subset of the other cage; returns true if the
   * two cages are equal.
   */
  bool isSubsetOf(const Cage<N> *other) const;
  bool isSupersetOf(const Cage<N> *other) const;

  Cage<N> operator-(const Cage<N> &other) const;

  template <uint32_t N_>
  friend std::ostream &operator<<(std::ostream &os, const Cage<N_> &cage);

protected:
  std::set<Cell<N> *> cells_; /*!< Cells which are members of this cage. */
  uint32_t sum_;              /*!< Total sum of the cage. */
};

template <uint32_t N> class LogicalCage : public Cage<N> {
public:
  LogicalCage() : LogicalCage({}, 0) {}
  LogicalCage(std::set<Cell<N> *> cells, const uint32_t sum);
  LogicalCage(std::set<Cage<N> *> cages);
  LogicalCage(const Cage<N> &cage);

  void init();
  void uninit();
  void registerNewOverlappingCage(LogicalCage<N> *cage);
  void registerNewSupersetCage(LogicalCage<N> *cage);
  void unregisterCage(LogicalCage<N> *cage);

  inline bool getUniqueness() const { return uniqueness_; }
  Cage<N> getMaximumSubset(const std::set<LogicalCage<N> *> &group) const;
  Cage<N> getMinimumSuperset(const std::set<LogicalCage<N> *> &group) const;
  inline bool isSuperset() const { return subsetCages_.size() > 0; }
  inline bool overlapsWith(const LogicalCage<N> *cage) const {
    return overlappingCages_.find(const_cast<LogicalCage<N> *>(cage)) !=
           overlappingCages_.end();
  }

  /**
   * Check whether a particular cell value will satisfy the constraints of this
   * cage. Since it is difficult to efficiently enforce uniqueness, if this cage
   * has a uniqueness constraint, it is only entirely enforced if this cage has
   * 3 or fewer cells.
   */
  bool testCellValues(const std::map<const Cell<N> *, uint32_t> pairs) const;

  template <uint32_t N_>
  friend std::ostream &operator<<(std::ostream &os,
                                  const LogicalCage<N_> &cage);

private:
  void updateUniqueness();

  bool uniqueness_; /*!< If true, then the values of cells in this cage are
                       required to be unique. */

  std::set<LogicalCage<N> *> overlappingCages_; /*!< Cache of cages which share
                                            at least one cell with this cage. */
  std::set<LogicalCage<N> *>
      subsetCages_; /*!< Cache of cages which are a subset of this cage. */
  std::set<LogicalCage<N> *>
      supersetCages_; /*!< Cache of cages which are a superset of this cage. */
};

template <uint32_t N>
std::ostream &operator<<(std::ostream &os, const Cage<N> &cage) {
  os << "Cage(sum: " << cage.getSum() << ", cells: {";
  for (const Cell<N> *cell : cage.getCells()) {
    os << *cell << ", ";
  }
  os << "})";
  return os;
}

template <uint32_t N>
std::ostream &operator<<(std::ostream &os, const LogicalCage<N> &cage) {
  os << "LogicalCage(sum: " << cage.getSum() << ", cells: {";
  for (const Cell<N> *cell : cage.getCells()) {
    os << *cell << ", ";
  }
  os << "}, uniqueness: " << cage.getUniqueness() << ", "
     << cage.overlappingCages_.size() << " overlapping, "
     << cage.subsetCages_.size() << " subset}";
  return os;
}
