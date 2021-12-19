#pragma once

#include <set>

template <typename T>
bool isSubset(const std::set<T> &parent, const std::set<T> &child) {
  bool all = true;
  for (const T &obj : child) {
    if (parent.find(obj) == parent.end()) {
      all = false;
      break;
    }
  }
  return all && parent.size() > child.size();
}
