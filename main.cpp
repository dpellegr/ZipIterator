#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "ZipIterator.hpp"
#include <chrono>


template<typename Clock>                                                                               
static inline double timediff2ms(std::chrono::time_point<Clock> start, std::chrono::time_point<Clock> stop) {
    if(stop < start) std::swap(stop, start);                                                           
    return std::chrono::duration<double, std::milli>(stop - start).count();                            
}                                                                                                      

int main(int argc, char **argv) {
  size_t nelem = 10000;
  if(argc > 1) {
    nelem = std::strtoull(argv[1], nullptr, 10);
  }
  std::vector<int> a{3,1,4,2};
  std::vector<std::string> b{"Alice","Bob","Charles","David"};

  auto zip = Zip(a,b);

  for (const auto & z: zip) std::cout << z << std::endl;
  std::cout << std::endl;

  std::sort(zip.begin(), zip.end());

  for (const auto & z: zip) std::cout << z << std::endl;


  std::vector<int> ai(nelem);
  std::iota(ai.begin(), ai.end(), 0);
  std::vector<std::string> bi(nelem);
  for(auto &i: bi) i = std::to_string(&i - bi.data());
  for(const auto &[x, y]: Zip(ai, bi)) {
    assert(std::to_string(x) == y);
  }
#if 1
  for(const auto &z: Zip(ai, bi)) {
    auto &[x,y] = z;
    assert(std::to_string(x) == y);
  }
#endif
  Zip zi(ai, bi);
  auto start = std::chrono::high_resolution_clock::now();
  std::sort(zi.begin(), zi.end(), [](const auto &x, const auto &y) {
    return std::get<1>(x) < std::get<1>(y);
  });
  auto stop = std::chrono::high_resolution_clock::now();
  assert(std::is_sorted(bi.begin(), bi.end()));
  assert(!std::is_sorted(ai.begin(), ai.end()));
  std::iota(ai.begin(), ai.end(), 0);
  for(auto &i: bi) i = std::to_string(&i - bi.data());
  std::fprintf(stderr, "Took %gms to sort iterator pairs\n", timediff2ms(start, stop));
  const Zip zi2(ai, bi);
  std::vector<std::pair<int, std::string>> vals;
  std::for_each(zi2.begin(), zi2.end(), [&](const auto &xy) {
      auto &x = std::get<0>(xy);
      auto &y = std::get<1>(xy);
      vals.emplace_back(x, y);
  });
  start = std::chrono::high_resolution_clock::now();
  std::sort(vals.begin(), vals.end(), [](const auto &x, const auto &y) {return x.second < y.second;});
  stop = std::chrono::high_resolution_clock::now();
  std::fprintf(stderr, "Took %gms to sort pairs\n", timediff2ms(start, stop));
  return 0;
}
