#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "ZipIterator.hpp"

int main() {
  std::vector<int> a{3,1,4,2};
  std::vector<std::string> b{"Alice","Bob","Charles","David"};

  auto zip = Zip(a,b);

  for (const auto & z: zip) std::cout << z << std::endl;
  std::cout << std::endl;

  std::sort(zip.begin(), zip.end());

  for (const auto & z: zip) std::cout << z << std::endl;


  std::vector<int> ai(1000000);
  std::iota(ai.begin(), ai.end(), 0);
  std::vector<std::string> bi(1000000);
  for(auto &i: bi) i = std::to_string(&i - bi.data());
  for(const auto &[x, y]: Zip(ai, bi)) {
    assert(std::to_string(x) == y);
  }
  Zip zi(ai, bi);
  std::sort(zi.begin(), zi.end(), [](const auto &x, const auto &y) {
    return std::get<1>(x) < std::get<1>(y);
  });
  assert(std::is_sorted(bi.begin(), bi.end()));
  assert(!std::is_sorted(ai.begin(), ai.end()));
  return 0;
}
