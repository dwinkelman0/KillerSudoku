#pragma once

#include "Cage.h"

#include <chrono>
#include <set>
#include <string.h>
#include <vector>

enum class SolverStatus { SOLVED, UNSOLVED, CONFLICT };

uint32_t CANONICAL_BOARD_9x9[81] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 1, 2, 3, 7, 8, 9,
    1, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 7, 8, 9, 1, 5, 6, 7, 8, 9, 1,
    2, 3, 4, 8, 9, 1, 2, 3, 4, 5, 6, 7, 3, 4, 5, 6, 7, 8, 9, 1, 2,
    6, 7, 8, 9, 1, 2, 3, 4, 5, 9, 1, 2, 3, 4, 5, 6, 7, 8};

const uint32_t BOARD_SIZE = 9;
const uint32_t BOARD_SIZE_SQRT = 3;
const uint32_t DEFAULT_SUM = BOARD_SIZE * (BOARD_SIZE + 1) / 2;
using Cell_t = Cell<BOARD_SIZE>;
using Cage_t = LogicalCage<BOARD_SIZE>;
using Group_t = std::set<Cage_t *>;

Group_t generateGenericCages(Cell_t *cells) {
  Group_t genericCages;
  for (int row = 0; row < BOARD_SIZE; ++row) {
    std::set<Cell<BOARD_SIZE> *> rowSet;
    std::set<Cell<BOARD_SIZE> *> colSet;
    for (int col = 0; col < BOARD_SIZE; ++col) {
      rowSet.insert(&cells[row * BOARD_SIZE + col]);
      colSet.insert(&cells[col * BOARD_SIZE + row]);
    }
    genericCages.insert(new Cage_t(rowSet, DEFAULT_SUM));
    genericCages.insert(new Cage_t(colSet, DEFAULT_SUM));
  }
  for (int row1 = 0; row1 < BOARD_SIZE_SQRT; ++row1) {
    for (int col1 = 0; col1 < BOARD_SIZE_SQRT; ++col1) {
      std::set<Cell<BOARD_SIZE> *> squareSet;
      for (int row2 = 0; row2 < BOARD_SIZE_SQRT; ++row2) {
        for (int col2 = 0; col2 < BOARD_SIZE_SQRT; ++col2) {
          squareSet.insert(&cells[(row1 * BOARD_SIZE_SQRT + row2) * BOARD_SIZE +
                                  col1 * BOARD_SIZE_SQRT + col2]);
        }
      }
      genericCages.insert(new Cage_t(squareSet, DEFAULT_SUM));
    }
  }
  return genericCages;
}

Group_t linkCages(Group_t &genericCages, Group_t &definedCages) {
  Group_t primaryCages(genericCages.begin(), genericCages.end());
  primaryCages.insert(definedCages.begin(), definedCages.end());
  for (Cage_t *cage : primaryCages) {
    cage->init();
  }
  Group_t differenceCages;
  for (const Cage_t *cage : genericCages) {
    Cage_t subset = cage->getMaximumSubset(definedCages);
    Cage_t *subsetDiff = new Cage_t(*cage - subset);
    subsetDiff->init();
    if (subsetDiff->getSum() > 0) {
      differenceCages.insert(subsetDiff);
    } else {
      subsetDiff->uninit();
      delete subsetDiff;
    }
    Cage_t superset = cage->getMinimumSuperset(definedCages);
    Cage_t *supersetDiff = new Cage_t(superset - *cage);
    supersetDiff->init();
    if (supersetDiff->getSum() > 0) {
      differenceCages.insert(supersetDiff);
    } else {
      supersetDiff->uninit();
      delete supersetDiff;
    }
  }
  Group_t allCages;
  allCages.insert(primaryCages.begin(), primaryCages.end());
  allCages.insert(differenceCages.begin(), differenceCages.end());
  return allCages;
}

std::vector<std::tuple<Cell_t *, Cell_t *, uint32_t>>
generateDifferentByOne(Group_t &allCages) {
  std::vector<std::tuple<Cell_t *, Cell_t *, uint32_t>> differentByOne;
  for (const Cage_t *cage1 : allCages) {
    for (const Cage_t *cage2 : allCages) {
      if (cage1->getSum() >= cage2->getSum() &&
          cage1->getNumCells() == cage2->getNumCells() &&
          cage1->overlapsWith(cage2)) {
        std::set<Cell_t *> allCells(cage1->getCells());
        for (Cell_t *cell : cage2->getCells()) {
          allCells.insert(cell);
        }
        if (allCells.size() == cage1->getNumCells() + 1) {
          Cell_t *upper = nullptr, *lower = nullptr;
          for (Cell_t *cell : cage1->getCells()) {
            if (*cage2->getCells().find(cell) == *cage2->getCells().end()) {
              upper = cell;
            }
          }
          for (Cell_t *cell : cage2->getCells()) {
            if (*cage1->getCells().find(cell) == *cage1->getCells().end()) {
              lower = cell;
            }
          }
          differentByOne.push_back(
              {upper, lower, cage1->getSum() - cage2->getSum()});
        }
      }
    }
  }
  return differentByOne;
}

SolverStatus
narrow(Cell_t *cells,
       std::vector<std::tuple<Cell_t *, Cell_t *, uint32_t>> &differentByOne) {
  bool progress = true;
  while (progress) {
    progress = false;
    for (auto &tuple : differentByOne) {
      if (std::get<0>(tuple)->narrowPossibleValues(std::get<1>(tuple),
                                                   std::get<2>(tuple))) {
        if (std::get<0>(tuple)->isConflict() ||
            std::get<1>(tuple)->isConflict()) {
          return SolverStatus::CONFLICT;
        }
        progress = true;
      }
    }
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
      if (cells[i].narrowPossibleValues()) {
        if (cells[i].isConflict()) {
          return SolverStatus::CONFLICT;
        }
        progress = true;
      }
    }
  }
  bool allSolved = true;
  for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
    if (!cells[i].isSolved()) {
      return SolverStatus::UNSOLVED;
    }
  }
  return SolverStatus::SOLVED;
}

Cell_t *chooseGuessCell(Cell_t *cells) {
  uint32_t numPossible = BOARD_SIZE + 1;
  uint32_t numCages = 0;
  Cell_t *bestCell = nullptr;
  for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
    if (cells[i].isSolved()) {
      continue;
    }
    uint32_t cellNumPossible = cells[i].getPossibleValues().count();
    if (cellNumPossible < numPossible ||
        (cellNumPossible == numPossible && cells[i].getNumCages() > numCages)) {
      numPossible = cellNumPossible;
      numCages = cells[i].getNumCages();
      bestCell = &cells[i];
    }
  }
  return bestCell;
}

uint32_t solveRecurse(
    Cell_t *cells,
    std::vector<std::tuple<Cell_t *, Cell_t *, uint32_t>> &differentByOne,
    uint32_t depth) {
  SolverStatus status = narrow(cells, differentByOne);
  if (status == SolverStatus::UNSOLVED) {
    // Make a guess
    uint32_t numSolutions = 0;
    Cell_t *guessCell = chooseGuessCell(cells);
    std::cout << "UNSOLVED: about to make a guess" << std::endl;
    for (uint32_t i : guessCell->getPossibleValues()) {
      std::cout << "Guessing cell " << *guessCell << " is " << i + 1
                << std::endl;
      for (int j = 0; j < BOARD_SIZE * BOARD_SIZE; ++j) {
        cells[j].saveState();
      }
      guessCell->manuallySetValue(i);
      numSolutions += solveRecurse(cells, differentByOne, depth + 1);
      for (int j = 0; j < BOARD_SIZE * BOARD_SIZE; ++j) {
        cells[j].restoreState();
      }
    }
    return numSolutions;
  } else if (status == SolverStatus::CONFLICT) {
    // Rewind
    if (depth == 0) {
      std::cout << "CONFLICT: no guesses have been made, so there is a serious "
                   "problem"
                << std::endl;
      return 0;
    }
    std::cout << "CONFLICT: about to undo a guess" << std::endl;
    return 0;
  } else {
    // Everything is solved
    std::cout << "SOLVED" << std::endl;
    return 1;
  }
  return 0;
}

uint32_t solve(Cell_t *cells, Group_t &definedCages) {
  auto start = std::chrono::high_resolution_clock::now();
  Group_t genericCages = generateGenericCages(cells);
  Group_t allCages = linkCages(genericCages, definedCages);
  auto differentByOne = generateDifferentByOne(allCages);
  uint32_t result = solveRecurse(cells, differentByOne, 0);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Running time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
  return result;
}
