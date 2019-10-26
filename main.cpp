#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "ZipIterator.hpp"

int main() {
  std::vector<int> a{3,1,4,2};
  std::vector<std::string> b{"Alice","Bob","Charles","David"};

  auto zip = Zip(a,b);

  for (const auto & z: zip) std::cout << z << std::endl;
  std::cout << std::endl;

  std::sort(zip.begin(), zip.end());

  for (const auto & z: zip) std::cout << z << std::endl;
  return 0;
}
