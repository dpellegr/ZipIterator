# ZipIterator
ZipIterator provides a variadic reference-based implementation of a zip iterator in C++(>=17).

The zip iterator primary target is the solution of a pretty common programming problem in which the permutation applied while sorting a container should be replicated to another container. Or, in other words, sorting a container according to the content of another container.

You can go with range-v3, which is a very powerfull library which has been partially standardized. However its zip iterator did not make it even in C++20, so here is my go to have a lightweight and easy to use implementation of the zip iterator.

# Usage Example
 Consider this minimal ezample:
  
    #include <vector>
    #include <iostream>
    #include "ZipIterator.hpp"

    int main() {
      std::vector<int> a{3,2,1};
      std::vector<std::string> b{"Alice","Bob","Charles"};

      std::sort(Zip(a,b).begin(), Zip(a,b).end());

      assert(a == std::vector<int>{1,2,3});
      assert(b == std::vector<std::string>{"Charles","Bob","Alice"};
      return 0;
    }

Here we managed to sort vector a, while at the same time, applying the same permutation to vector b.
In particular Zip construct a tuple of iterators of the specified containers and handle its dereferentiation to a tuple of plain references to the elements of the containers. The swap operation then affect the content of the original containers and the comparison are standard lexicographic tuple comparisons.

Printing the content of zipped containers is easy:

    for (const auto& z: Zip(a,b)) std::cout << z << '\n';

The old iterator style for loop is also possible:

    for (auto z=std::begin(a,b); z < std::end(a,b); ++z) {
      std::cout << *z << '\n';
    }

# Notes
