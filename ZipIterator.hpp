#include <iterator>
#include <tuple>
#include <memory>
#include <sstream>

template <typename ...IT>
class ZipIter {
public:
  using iterator_category = typename std::iterator_traits<
                              typename std::tuple_element<0, std::tuple<IT...>>::type
                            >::iterator_category;
  using value_type        = std::tuple<typename std::iterator_traits<IT>::value_type ...>;
  using difference_type   = typename std::iterator_traits<
                              typename std::tuple_element<0, std::tuple<IT...>>::type
                            >::difference_type;
  using pointer           = std::tuple<typename std::iterator_traits<IT>::pointer ...>;
  using reference         = std::tuple<typename std::iterator_traits<IT>::reference ...>;

private:
  std::tuple<IT...> zip;

public:
  ZipIter(const std::tuple<IT...>& z): zip(z) {}
  ZipIter(IT... z): zip(std::make_tuple(z...)) {}
  ZipIter(const ZipIter& z): zip(z.zip) {}
  
  ZipIter& operator=(const ZipIter& z) { zip = z.zip; return *this;}
  ZipIter& operator=(ZipIter&& z) { zip = std::move(z.zip); return *this;}

  template<typename F>
  ZipIter& apply(F&& f){ std::apply([&](auto&&... args){ ((*args = f(*args)), ...); }, zip ); return *this; }

  ZipIter& operator+=(const long n) { 
    std::apply([n](auto&&...args){ ((std::advance(args,n)),...); }, zip );
    return *this;
  }
  ZipIter& operator-=(const long n) { return this->operator+=(-n); }
  ZipIter& operator++()    { return this->operator+=(1); }
  ZipIter& operator--()    { return this->operator-=(1); }
  ZipIter  operator++(int) { ZipIter tmp(*this); operator++(); return tmp; }
  ZipIter  operator--(int) { ZipIter tmp(*this); operator--(); return tmp; }
  
  ZipIter operator+(const long n) const { auto res=*this; res+=n; return res; }
  ZipIter operator-(const long n) const { return *this + (-n); }

  bool operator==(const ZipIter& rhs) const {return zip==rhs.zip; }
  bool operator< (const ZipIter& rhs) const {return zip< rhs.zip; }
  bool operator!=(const ZipIter& rhs) const {return !this->operator==(rhs);}
  bool operator> (const ZipIter& rhs) const {return    rhs.operator< (*this);}
  bool operator<=(const ZipIter& rhs) const {return !this->operator> (rhs);}
  bool operator>=(const ZipIter& rhs) const {return !this->operator< (rhs);}

  reference  operator*()       {return std::apply([](auto&&... args){ return reference ( (*args)... );}, zip); }
  value_type operator*() const {return std::apply([](auto&&... args){ return value_type( (*args)... );}, zip); }

  reference  operator[](const long n)       {return *(this->operator+(n));}
  value_type operator[](const long n) const {return *(this->operator+(n));}

  template<size_t I=0> auto& get() {return *(std::get<I>(zip));}
  template<size_t I=0> auto& iter_get() {return std::get<I>(zip);}
  template<size_t I=0> auto get() const {return *(std::get<I>(zip));}
  template<size_t I=0> auto iter_get() const {return std::get<I>(zip);}

};

template <typename ...T>
inline auto operator-(const ZipIter<T...> & lhs, const ZipIter<T...>& rhs) {
  return std::distance( lhs.template iter_get<0>(), rhs.template iter_get<0>());
}

namespace std{
  template< class Ch, class Tr, template<class...> class TUP, class... Args>
  auto& operator<<(std::basic_ostream<Ch, Tr>& os, const TUP<Args...>& t) {
    std::basic_stringstream<Ch, Tr> ss;
    ss << "[ ";
    std::apply([&ss](auto&&... args) {((ss << args << ", "), ...);}, t);
    ss.seekp(-2, ss.cur); 
    ss << " ]";
    return os << ss.str();
  }

  //custom std::swap for tuple of references (passed by value), as returned when dereferencing ZipIter
  template <size_t I = 0, typename... T>
  void swap(std::tuple<T&...> a, std::tuple<T&...> b) {
    swap(get<I>(a),get<I>(b));
    if constexpr(I+1 != sizeof...(T))
      swap<I+1>(a, b);
  }

  template <typename ...T> auto  begin(T&... containers){ return ZipIter( std::begin(containers)...);}
  template <typename ...T> auto    end(T&... containers){ return ZipIter(   std::end(containers)...);}
  template <typename ...T> auto rbegin(T&... containers){ return ZipIter(std::rbegin(containers)...);}
  template <typename ...T> auto   rend(T&... containers){ return ZipIter(  std::rend(containers)...);}
}

template<typename ...Container>
class Zip {
  std::tuple<Container&...> zip;
public:
  Zip(Container&... z): zip(std::tie(z...)) {}
  Zip(const Zip& z): zip(z.zip) {}

  auto  begin(){return std::apply([](auto&&... args){ return ZipIter((args. begin())...);}, zip);}
  auto    end(){return std::apply([](auto&&... args){ return ZipIter((args.   end())...);}, zip);}
  auto rbegin(){return std::apply([](auto&&... args){ return ZipIter((args.rbegin())...);}, zip);}
  auto   rend(){return std::apply([](auto&&... args){ return ZipIter((args.  rend())...);}, zip);}
};
