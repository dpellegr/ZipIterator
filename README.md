# ZipIterator
ZipIterator provides a variadic reference-based implementation of a zip iterator in C++(>=17).

The zip iterator primary target is the solution of a pretty common programming problem in which the permutation applied when sorting a container, should be replicated to another container. Or, in other words, sorting a container according to the content of another container.

Historically this is approached either copying the data back and forth between arrays of structures and structures of arrays, or by instantiating a container of indexes which allows reproducing the sorting permutation on other containers.

Both the approaches are suboptimal: wouldn't it be nice if one could just sort the data in place without having to instantiate additional memory? This is exacte ZipIterator has been designed for this purpose!

# Example
 Consider this minimal example:
  
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

 The produced output is:

    $ ./main.out
    [ 3, Alice ]
    [ 1, Bob ]
    [ 4, Charles ]
    [ 2, David ]

    [ 1, Bob ]
    [ 2, David ]
    [ 3, Alice ]
    [ 4, Charles ]

What appened behind the curtain was that the same permutations applied by `std::sort` to the elements of vector `a` were simultaneously applied to vector `b` as well.

Note that it would have been possible to zip a third (and more) vector as well, as the implementation leverages on variadic templates.

# Details

The ZipIter class maintains a tuple of iterators of the specified containers, and handles its dereferentiation to a tuple of pointers to the original data, packed into the class ZipRef.

Here is where the magic take place: the data pointed by ZipRef can be mutable even if ZipRef itself is constant. This allow extending the lifetime of non-const lvalues references of ZipRef (as returned when dereferencing ZipIter) by binding them to const references, while still being able to modify the data being pointed to.

The helper class Zip, packages a tuple of reference to the specified containers and provides syntactic sugar for quick generation of ZipIter objects.

# Usage Notes

As internally tuples are used, all the comparison between the zipped containers proceed in a lexicographical fashion. Therefore when sorting it is important to pass first the container that will be ordered while the other(s) will follow.

In addition the first passed container defines the iterator_category of the resulting ZipIter object, and is used when computing differences of ZipIter.

Attempting any operation between ZipIter of different types is undefined behaviour.
Even when types are consistent, no range checks are implemented, the containers being manipulated should have the same size.
