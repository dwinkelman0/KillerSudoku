#pragma once

#include <bitset>
#include <iostream>

template <uint32_t N> class PossibleValues : public std::bitset<N> {
public:
  class Value {
  public:
    Value(const PossibleValues<N> &parent) : parent_(parent), index_(0) {
      for (; index_ < N && !parent_.test(index_); ++index_) {
        // Do nothing
      }
    }
    void setAsEnd() { index_ = N; }
    inline uint32_t operator*() const { return index_; }
    inline void operator++() {
      for (index_++; index_ < N && !parent_.test(index_); ++index_) {
        // Do nothing
      }
    }
    inline bool operator!=(const Value &other) const {
      if (parent_ != other.parent_) {
        return false;
      }
      if (index_ < N && other.index_ < N) {
        return index_ != other.index_;
      }
      return index_ < N || other.index_ < N;
    }

  private:
    PossibleValues<N> parent_;
    uint32_t index_;
  };

public:
  PossibleValues() : std::bitset<N>(0) {}
  PossibleValues(const std::bitset<N> &bitset) : std::bitset<N>(bitset) {}

  inline bool operator<(const PossibleValues &other) const {
    if (this->count() == other.count()) {
      return std::bitset<N>::operator<(other);
    }
    return this->count() < other.count();
  }

  Value begin() const { return Value(*this); }
  Value end() const {
    Value output(*this);
    output.setAsEnd();
    return output;
  }

  template <uint32_t N_>
  friend std::ostream &operator<<(std::ostream &os,
                                  const PossibleValues<N_> &values);
};

template <uint32_t N>
std::ostream &operator<<(std::ostream &os, const PossibleValues<N> &values) {
  os << "{";
  bool atLeastOne = false;
  for (int i = 0; i < N; ++i) {
    if (values.test(i)) {
      if (atLeastOne) {
        os << ", ";
      }
      os << i + 1;
      atLeastOne = true;
    }
  }
  os << "}";
  return os;
}
