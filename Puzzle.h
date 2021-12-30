#pragma once

#include "Cage.h"
#include "Cell.h"

#include <map>
#include <set>
#include <stdint.h>

template <uint32_t R, uint32_t C> class Puzzle {
public:
  static const uint32_t N = R * C;

  Puzzle();

  class Solution {
  public:
    Solution();

    inline uint32_t get(const uint32_t row, const uint32_t col) const {
      return data_[row * N + col];
    }

  protected:
    uint32_t data_[N * N];
  };

  class CellMap {
  public:
    CellMap(const std::vector<Cage<N> *> &cages);
    bool merge(Cell<N> *cell1, Cell<N> *cell2, const uint32_t maxCageSize);
    void unmerge();
    Cage<N> *getCage(Cell<N> *cell) const;
    inline uint32_t getNumCages() const { return activeCages_.size(); }
    inline std::set<LogicalCage<N> *> generateLogicalCages() const;

  protected:
    bool areInSameCage(Cell<N> *cell1, Cell<N> *cell2) const;
    void restoreCage(Cage<N> *cage);

    std::map<Cell<N> *, Cage<N> *> cageMap_;
    std::set<Cage<N> *> activeCages_;
    std::stack<std::pair<Cage<N> *, Cage<N> *>> history_;
  };

protected:
  std::vector<Cell<N> *> cells_;
};
