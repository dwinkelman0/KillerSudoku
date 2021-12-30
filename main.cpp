#include "Cage.h"
#include "Cell.h"
#include "Puzzle.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  // Cell_t cells[BOARD_SIZE * BOARD_SIZE];
  // Group_t definedCages = {
  // new Cage_t({&cells[0 * BOARD_SIZE + 0], &cells[1 * BOARD_SIZE + 0],
  //             &cells[2 * BOARD_SIZE + 0]},
  //            12),
  // new Cage_t({&cells[0 * BOARD_SIZE + 1], &cells[1 * BOARD_SIZE + 1]}, 10),
  // new Cage_t({&cells[0 * BOARD_SIZE + 2], &cells[1 * BOARD_SIZE + 2]}, 14),
  // new Cage_t({&cells[0 * BOARD_SIZE + 3], &cells[0 * BOARD_SIZE + 4],
  //             &cells[0 * BOARD_SIZE + 5]},
  //            21),
  // new Cage_t({&cells[0 * BOARD_SIZE + 6], &cells[1 * BOARD_SIZE + 5],
  //             &cells[1 * BOARD_SIZE + 6]},
  //            17),
  // new Cage_t({&cells[0 * BOARD_SIZE + 7], &cells[0 * BOARD_SIZE + 8],
  //             &cells[1 * BOARD_SIZE + 8]},
  //            14),
  // new Cage_t({&cells[1 * BOARD_SIZE + 3], &cells[1 * BOARD_SIZE + 4],
  //             &cells[2 * BOARD_SIZE + 4]},
  //            8),
  // new Cage_t({&cells[1 * BOARD_SIZE + 7], &cells[2 * BOARD_SIZE + 7]}, 6),
  // new Cage_t({&cells[2 * BOARD_SIZE + 1], &cells[2 * BOARD_SIZE + 2]}, 9),
  // new Cage_t({&cells[2 * BOARD_SIZE + 3], &cells[3 * BOARD_SIZE + 3]}, 14),
  // new Cage_t({&cells[2 * BOARD_SIZE + 5], &cells[3 * BOARD_SIZE + 5],
  //             &cells[3 * BOARD_SIZE + 6]},
  //            11),
  // new Cage_t({&cells[2 * BOARD_SIZE + 6]}, 7),
  // new Cage_t({&cells[2 * BOARD_SIZE + 8], &cells[3 * BOARD_SIZE + 7],
  //             &cells[3 * BOARD_SIZE + 8], &cells[4 * BOARD_SIZE + 7]},
  //            26),
  // new Cage_t({&cells[3 * BOARD_SIZE + 0], &cells[4 * BOARD_SIZE + 0],
  //             &cells[5 * BOARD_SIZE + 0]},
  //            17),
  // new Cage_t({&cells[3 * BOARD_SIZE + 1], &cells[3 * BOARD_SIZE + 2],
  //             &cells[4 * BOARD_SIZE + 1]},
  //            14),
  // new Cage_t({&cells[3 * BOARD_SIZE + 4], &cells[4 * BOARD_SIZE + 4]}, 12),
  // new Cage_t({&cells[4 * BOARD_SIZE + 2], &cells[4 * BOARD_SIZE + 3],
  //             &cells[5 * BOARD_SIZE + 3]},
  //            10),
  // new Cage_t({&cells[4 * BOARD_SIZE + 5], &cells[5 * BOARD_SIZE + 5],
  //             &cells[6 * BOARD_SIZE + 5]},
  //            19),
  // new Cage_t({&cells[4 * BOARD_SIZE + 6], &cells[5 * BOARD_SIZE + 6],
  //             &cells[6 * BOARD_SIZE + 6], &cells[7 * BOARD_SIZE + 6]},
  //            18),
  // new Cage_t({&cells[4 * BOARD_SIZE + 8]}, 7),
  // new Cage_t({&cells[5 * BOARD_SIZE + 1], &cells[5 * BOARD_SIZE + 2],
  //             &cells[6 * BOARD_SIZE + 2]},
  //            16),
  // new Cage_t({&cells[5 * BOARD_SIZE + 4], &cells[6 * BOARD_SIZE + 4]}, 11),
  // new Cage_t({&cells[5 * BOARD_SIZE + 7], &cells[6 * BOARD_SIZE + 7]}, 7),
  // new Cage_t({&cells[5 * BOARD_SIZE + 8], &cells[6 * BOARD_SIZE + 8]}, 11),
  // new Cage_t({&cells[6 * BOARD_SIZE + 0], &cells[6 * BOARD_SIZE + 1],
  //             &cells[7 * BOARD_SIZE + 0], &cells[7 * BOARD_SIZE + 1]},
  //            15),
  // new Cage_t({&cells[6 * BOARD_SIZE + 3], &cells[7 * BOARD_SIZE + 3]}, 7),
  // new Cage_t({&cells[7 * BOARD_SIZE + 2], &cells[8 * BOARD_SIZE + 2],
  //             &cells[8 * BOARD_SIZE + 3]},
  //            21),
  // new Cage_t({&cells[7 * BOARD_SIZE + 4], &cells[8 * BOARD_SIZE + 4]}, 8),
  // new Cage_t({&cells[7 * BOARD_SIZE + 5]}, 9),
  // new Cage_t({&cells[7 * BOARD_SIZE + 7], &cells[7 * BOARD_SIZE + 8]}, 9),
  // new Cage_t({&cells[8 * BOARD_SIZE + 0], &cells[8 * BOARD_SIZE + 1]}, 10),
  // new Cage_t({&cells[8 * BOARD_SIZE + 5], &cells[8 * BOARD_SIZE + 6]}, 5),
  // new Cage_t({&cells[8 * BOARD_SIZE + 7], &cells[8 * BOARD_SIZE + 8]}, 10)};
  // std::set<Cage_t *> definedCages{
  //     new Cage_t({&cells[0 * BOARD_SIZE + 0], &cells[0 * BOARD_SIZE + 1],
  //                 &cells[0 * BOARD_SIZE + 2]},
  //                15),
  //     new Cage_t({&cells[0 * BOARD_SIZE + 3], &cells[0 * BOARD_SIZE + 4],
  //                 &cells[0 * BOARD_SIZE + 5], &cells[0 * BOARD_SIZE + 6]},
  //                26),
  //     new Cage_t({&cells[0 * BOARD_SIZE + 7], &cells[0 * BOARD_SIZE + 8]},
  //     4), new Cage_t({&cells[1 * BOARD_SIZE + 0], &cells[1 * BOARD_SIZE +
  //     1]}, 11), new Cage_t({&cells[1 * BOARD_SIZE + 2], &cells[1 * BOARD_SIZE
  //     + 3],
  //                 &cells[2 * BOARD_SIZE + 3]},
  //                9),
  //     new Cage_t({&cells[1 * BOARD_SIZE + 4], &cells[2 * BOARD_SIZE + 4]},
  //     6), new Cage_t({&cells[1 * BOARD_SIZE + 5], &cells[1 * BOARD_SIZE +
  //     6]}, 13), new Cage_t({&cells[1 * BOARD_SIZE + 7], &cells[1 * BOARD_SIZE
  //     + 8],
  //                 &cells[2 * BOARD_SIZE + 7]},
  //                15),
  //     new Cage_t({&cells[2 * BOARD_SIZE + 0], &cells[2 * BOARD_SIZE + 1]},
  //     9), new Cage_t({&cells[2 * BOARD_SIZE + 2], &cells[3 * BOARD_SIZE + 2],
  //                 &cells[3 * BOARD_SIZE + 3]},
  //                17),
  //     new Cage_t({&cells[2 * BOARD_SIZE + 5], &cells[2 * BOARD_SIZE + 6]},
  //     11), new Cage_t({&cells[2 * BOARD_SIZE + 8], &cells[3 * BOARD_SIZE +
  //     8]}, 9), new Cage_t({&cells[3 * BOARD_SIZE + 0], &cells[3 * BOARD_SIZE
  //     + 1]}, 5), new Cage_t({&cells[3 * BOARD_SIZE + 4], &cells[3 *
  //     BOARD_SIZE + 5],
  //                 &cells[3 * BOARD_SIZE + 6]},
  //                24),
  //     new Cage_t({&cells[3 * BOARD_SIZE + 7], &cells[4 * BOARD_SIZE + 7],
  //                 &cells[4 * BOARD_SIZE + 8]},
  //                17),
  //     new Cage_t({&cells[4 * BOARD_SIZE + 0]}, 9),
  //     new Cage_t({&cells[4 * BOARD_SIZE + 1], &cells[4 * BOARD_SIZE + 2]},
  //     10), new Cage_t({&cells[4 * BOARD_SIZE + 3], &cells[5 * BOARD_SIZE +
  //     3]}, 9), new Cage_t({&cells[4 * BOARD_SIZE + 4], &cells[4 * BOARD_SIZE
  //     + 5]}, 7), new Cage_t({&cells[4 * BOARD_SIZE + 6], &cells[5 *
  //     BOARD_SIZE + 6]}, 4), new Cage_t({&cells[5 * BOARD_SIZE + 0], &cells[6
  //     * BOARD_SIZE + 0],
  //                 &cells[7 * BOARD_SIZE + 0]},
  //                11),
  //     new Cage_t({&cells[5 * BOARD_SIZE + 1], &cells[5 * BOARD_SIZE + 2]},
  //     9), new Cage_t({&cells[5 * BOARD_SIZE + 4], &cells[5 * BOARD_SIZE +
  //     5]}, 11), new Cage_t({&cells[5 * BOARD_SIZE + 7], &cells[6 * BOARD_SIZE
  //     + 7]}, 9), new Cage_t({&cells[5 * BOARD_SIZE + 8], &cells[6 *
  //     BOARD_SIZE + 8],
  //                 &cells[7 * BOARD_SIZE + 7], &cells[7 * BOARD_SIZE + 8]},
  //                28),
  //     new Cage_t({&cells[6 * BOARD_SIZE + 1], &cells[7 * BOARD_SIZE + 1]},
  //     14), new Cage_t({&cells[6 * BOARD_SIZE + 2], &cells[7 * BOARD_SIZE +
  //     2]}, 13), new Cage_t({&cells[6 * BOARD_SIZE + 3], &cells[6 * BOARD_SIZE
  //     + 4]}, 11), new Cage_t({&cells[6 * BOARD_SIZE + 5], &cells[6 *
  //     BOARD_SIZE + 6],
  //                 &cells[7 * BOARD_SIZE + 6], &cells[8 * BOARD_SIZE + 6]},
  //                18),
  //     new Cage_t({&cells[7 * BOARD_SIZE + 3], &cells[8 * BOARD_SIZE + 3]},
  //     9), new Cage_t({&cells[7 * BOARD_SIZE + 4], &cells[7 * BOARD_SIZE +
  //     5]}, 5), new Cage_t({&cells[8 * BOARD_SIZE + 0], &cells[8 * BOARD_SIZE
  //     + 1],
  //                 &cells[8 * BOARD_SIZE + 2]},
  //                14),
  //     new Cage_t({&cells[8 * BOARD_SIZE + 4], &cells[8 * BOARD_SIZE + 5]},
  //     15), new Cage_t({&cells[8 * BOARD_SIZE + 7], &cells[8 * BOARD_SIZE +
  //     8]}, 8)};

  // for (int i = 0; i < BOARD_SIZE; ++i) {
  //   for (int j = 0; j < BOARD_SIZE; ++j) {
  //     Cell_t *cell = &cells[i * BOARD_SIZE + j];
  //     std::cout << "\t" << *cell << ": {";
  //     for (int i = 0; i < BOARD_SIZE; ++i) {
  //       if (cell->getPossibleValues().test(i)) {
  //         std::cout << i + 1 << ", ";
  //       }
  //     }
  //     std::cout << "}" << std::endl;
  //   }
  // }

  // std::vector<Cell_t *> cellVector;
  // for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
  //   cellVector.push_back(&cells[i]);
  // }
  // for (Cage_t *cage : definedCages) {
  //   cage->init();
  // }
  // uint32_t numSolutions = solve(cellVector, definedCages);
  // std::cout << numSolutions << " solutions found" << std::endl;
  // Puzzle<3, 3>::Solution sol;
  // for (int i = 0; i < 9; ++i) {
  //   for (int j = 0; j < 9; ++j) {
  //     std::cout << std::setw(1) << sol.get(i, j) << ", ";
  //   }
  //   std::cout << std::endl;
  // }

  Puzzle<3, 3> puzzle;

  return 0;
}
