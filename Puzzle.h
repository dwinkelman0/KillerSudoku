#pragma once

#include <stdint.h>

template <uint32_t R, uint32_t C>
class Puzzle {
 public:
  static const uint32_t N = R * C;

  class Solution {
    public:
    Solution();

    inline uint32_t get(const uint32_t row, const uint32_t col) const {
      return data_[row * N + col];
    }

    protected:
     uint32_t data_[N * N];
  };
};
