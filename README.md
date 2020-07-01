# C++17 ZipIterator
***Note** Althought nice and compact, this implementation ignores a C++ specification regarding `iterator_category` and may give undefined behaviour. Most of the implementations of the standard library do not rely on that specifications. Indeed many find it over-restrictive and it has been proposed for dropping from future C++ standards. Anyway, please thoroughly test your user case/environment before relying on it. For more info: https://codereview.stackexchange.com/questions/231352/c17-zip-iterator-compatible-with-stdsort*

ZipIterator provides a variadic pointer-based implementation of the zip iterator pattern in C++(>=17).

One of the main problems targeted by the zip iterator consists of sorting a container while replicating the same permutations to another container. Or, in other words, sorting a container according to the content of another container.

Historically this problem has often been approached in C++ either by copying the data back and forth between arrays of structures and structures of arrays, or by instantiating a container of indexes to keep track of the permutations.

Both the approaches are suboptimal: wouldn't it be nice if one could just sort the data in place without having to instantiate additional memory except for the one needed to perform the swaps operations? This is exactly what ZipIterator has been designed for!

# Example
 Consider this minimal example:
```c++
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "ZipIterator.hpp" //Header only ;)

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
```
It can be compiled by simply enabling the c++17 (or more recent) standard and it produces the following output:
```bash
$ g++ -std=c++17 main.cpp -o main.out && ./main.out
[ 3, Alice ]
[ 1, Bob ]
[ 4, Charles ]
[ 2, David ]

[ 1, Bob ]
[ 2, David ]
[ 3, Alice ]
[ 4, Charles ]
```
Behind the curtain the permutations (either by swapping or copying) applied by `std::sort` to the elements of vector `a` have been simultaneously applied to vector `b`.

Note that it would have been possible to zip a third (and more) vector as well, as the implementation leverages on variadic templates.

# Details

The `ZipIter` class maintains a tuple of iterators of the specified containers, and handles its dereferentiation to a tuple of pointers to the original data, packed into the class `ZipRef`.

Here is where the magic take place: the data pointed by `ZipRef` can be mutable even if `ZipRef` itself is constant. This allow extending the lifetime of non-const lvalues references of `ZipRef` (as the one returned when dereferencing `ZipIter`) by binding them to const references, while still being able, later, to modify the data being pointed to.

The helper class `Zip` provides syntactic sugar for quick generation of `ZipIter` objects by automatizing the creation of the tuple of references from the specified containers.

# Usage Notes

As internally tuples are used, all the comparisons between the zipped containers proceed in a lexicographical fashion. Therefore the container that is passed first has a special relevance: for instance, it will define the order of the zipped iterators when sorting. In addition it defines the `iterator_category` of the resulting `ZipIter` object (which may or may not be consistent with the other zipped iterators), and it is used when computing distances.

Attempting any operation between ZipIters of different types is undefined behavior.
Even when types are consistent, no range checks are implemented: it is up to the user to ensure that the containers being manipulated have the same size although this is may not be a strict requirement depending on how the iterator is then used.
