# ZipIterator
ZipIterator provides a variadic reference-based implementation of a zip iterator in C++(>=17).

The zip iterator primary target is the solution of a pretty common programming problem in which the permutation applied while sorting a container should be replicated to another container. Or, in other words, sorting a container according to the content of another container.

You can go with range-v3, which is a very powerfull library which has been partially standardized. However its implementation of the zip iterator did not make it even in C++20, so here is my attempt to produce a lightweight and easy to use implementation of the zip iterator.

# Usage Example
 Consider this minimal example:
  
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

Here we sort vector a, and, at the same time, we apply the same permutation to vector b.

Printing the content of zipped containers is easy:

    for (const auto& z: Zip(a,b)) std::cout << z << '\n';

The old iterator style for loop is also possible:

    for (auto z=std::begin(a,b); z < std::end(a,b); ++z) {
      std::cout << *z << '\n';
    }

# Details

The ZipIter class maintains a tuple of iterators of the specified containers, and handles its dereferentiation to a tuple of plain references to the elements of the original containers.
The helper class Zip, packages a tuple of reference to the specified containers and allow for quick generation of ZipIter objects.

# Notes

As internally tuples are used, all the comparison between the zipped containers proceed in a lexicographical fashion. Therefore when sorting it is important to pass first the container that will be ordered while the other(s) will follow.

In addition the first passed container defines the iterator_category of the resulting ZipIter object, and is used when computing differences of ZipIter.

No range checks are implemented. It is up to the user to pass containers with the same size.
