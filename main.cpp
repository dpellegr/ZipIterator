#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "ZipIterator.hpp"

int main() {
  std::vector<int> a(1000000);
  std::iota(a.begin(), a.end(), 0);
  std::vector<std::string> b(1000000);
    for(auto &i: b) i = std::to_string(&i - b.data());
  auto z = Zip(a, b);
  for(const auto &[x, y]: z) {
    assert(std::to_string(x) == y);
  }
  return 0;
}
