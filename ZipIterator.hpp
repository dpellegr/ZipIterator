//
// C++17 implementation of ZipIterator by Dario Pellegrini <pellegrini.dario@gmail.com>
// Still unsure about the licence, but something in the line of just providing attribution
// October 2019
//

#include <tuple>

template <typename ...T>
class ZipRef {
  std::tuple<T*...> ptr;
public:
  ZipRef() = delete;
  ZipRef(const ZipRef& z) = default;
  ZipRef(ZipRef&& z) = default;
  ZipRef(T* const... p): ptr(p...) {}

  ZipRef& operator=(const ZipRef& z)             { return copy_assign( z); }
  ZipRef& operator=(const std::tuple<T...>& val) { return val_assign(val); }

  template <size_t I = 0>
  ZipRef& copy_assign(const ZipRef& z) {
    *(std::get<I>(ptr)) = *(std::get<I>(z.ptr));
    if constexpr( I+1 < sizeof...(T) ) return copy_assign<I+1>(z);
    return *this;
  }
  template <size_t I = 0>
  ZipRef& val_assign(const std::tuple<T...>& t) {
    *(std::get<I>(ptr)) = std::get<I>(t);
    if constexpr( I+1 < sizeof...(T) ) return val_assign<I+1>(t);
    return *this;
  }

  std::tuple<T...> val() const {return std::apply([](auto&&...args){ return std::tuple((*args)...); }, ptr);}
  operator std::tuple<T...>() const { return val(); }

  template <size_t I = 0>
  void swap(const ZipRef& o) const {
    std::swap(*(std::get<I>(ptr)), *(std::get<I>(o.ptr)));
    if constexpr( I+1 < sizeof...(T) ) swap<I+1>(o);
  }

  #define OPERATOR(OP) \
    bool operator OP(const ZipRef & o) const { return val() OP o.val(); } \
    inline friend bool operator OP(const ZipRef& r, const std::tuple<T...>& t) { return r.val() OP t; } \
    inline friend bool operator OP(const std::tuple<T...>& t, const ZipRef& r) { return t OP r.val(); }

    OPERATOR(==) OPERATOR(<=) OPERATOR(>=)
    OPERATOR(!=) OPERATOR(<)  OPERATOR(>)
  #undef OPERATOR

};

template<typename ...IT>
class ZipIter {
  std::tuple<IT...> it;

  template <size_t I = 0, typename F>
  void apply(F&& f) {
    f(std::get<I>(it));
    if constexpr( I+1 < sizeof...(IT) ) apply<I+1>(f);
  }

  template<int N, typename... Ts> using NthTypeOf =
    typename std::tuple_element<N, std::tuple<Ts...>>::type;
  template<typename... Ts> using FirstTypeOf = NthTypeOf<0, Ts...>;

public:
  using iterator_category = typename std::iterator_traits<FirstTypeOf<IT...>>::iterator_category;
  using difference_type   = typename std::iterator_traits<FirstTypeOf<IT...>>::difference_type;
  using value_type        = std::tuple<typename std::iterator_traits<IT>::value_type ...>;
  using pointer           = std::tuple<typename std::iterator_traits<IT>::pointer ...>;
  using reference         = ZipRef<typename std::iterator_traits<IT>::value_type ...>;

  ZipIter() = default;
  ZipIter(const ZipIter &rhs) = default;
  ZipIter(const IT&... rhs): it(rhs...) {}

  ZipIter& operator=(const ZipIter &rhs) { it = rhs.it; return *this;}
  ZipIter& operator+=(const difference_type d) { apply([&d](auto&&it){std::advance(it, d);}); return *this;}
  ZipIter& operator-=(const difference_type d) { apply([&d](auto&&it){std::advance(it,-d);}); return *this;}

  reference operator* () const {return std::apply([](auto&&...args){return reference(&(*(args))...);}, it);}
  pointer   operator->() const {return std::apply([](auto&&...args){return pointer  (&(*(args))...);}, it);}
  reference operator[](difference_type rhs) const {return *(operator+(rhs));}

  ZipIter& operator++() { apply([](auto&&it){++it;}); return *this;}
  ZipIter& operator--() { apply([](auto&&it){--it;}); return *this;}
  ZipIter operator++(int) {ZipIter tmp(*this); operator++(); return tmp;}
  ZipIter operator--(int) {ZipIter tmp(*this); operator--(); return tmp;}

  difference_type operator-(const ZipIter& rhs) const {return std::get<0>(it)-std::get<0>(rhs.it);}
  ZipIter operator+(const difference_type d) const {ZipIter tmp(*this); tmp += d; return tmp;}
  ZipIter operator-(const difference_type d) const {ZipIter tmp(*this); tmp -= d; return tmp;}
  inline friend ZipIter operator+(const difference_type d, const ZipIter& z) {return z+d;}
  inline friend ZipIter operator-(const difference_type d, const ZipIter& z) {return z-d;}

  #define OPERATOR(OP) \
    bool operator OP(const ZipIter& rhs) const {return it OP rhs.it;}
    OPERATOR(==) OPERATOR(<=) OPERATOR(>=)
    OPERATOR(!=) OPERATOR(<)  OPERATOR(>)
  #undef OPERATOR
};

template<typename ...Container>
class Zip {
  std::tuple<Container&...> zip;

public:
  Zip() = delete;
  Zip(const Zip& z) = default;
  Zip(Zip&& z) = default;
  Zip(Container&... z): zip(z...) {}

  #define HELPER(OP) \
    auto OP(){return std::apply([](auto&&... args){ return ZipIter((args.OP())...);}, zip);}
    HELPER( begin) HELPER( end)
    HELPER(rbegin) HELPER(rend)
  #undef HELPER
};

#include <utility>
using std::swap;
template<typename ...T> void swap(const ZipRef<T...>& a, const ZipRef<T...>& b) { a.swap(b); }

#include <iostream>
#include <sstream>
template< class Ch, class Tr, class...IT, typename std::enable_if<(sizeof...(IT)>0), int>::type = 0>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, const ZipRef<IT...>& t) {
  std::basic_stringstream<Ch, Tr> ss;
  ss << "[ ";
  std::apply([&ss](auto&&... args) {((ss << args << ", "), ...);}, t.val());
  ss.seekp(-2, ss.cur);
  ss << " ]";
  return os << ss.str();
}

