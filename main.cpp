#ifdef _MSC_VER
#define _ITERATOR_DEBUG_LEVEL (0)
#define _HAS_ITERATOR_DEBUGGING (0)
#endif

#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <iterator>
#include <algorithm>

#include "custom_heap.h"

/**
 * @brief Шаблон аллокатора с параметрически заданным количеством элементов.
 */
template <typename T, size_t N = 0>
class CustomAllocator
{
  public:
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    CustomAllocator() {
      std::cout << "Ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    ~CustomAllocator() {
      std::cout << "Dtor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    CustomAllocator(const CustomAllocator&) {
      std::cout << "Copy ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    pointer allocate(size_type n, const void* = 0) {
      auto ptr = takeBlock(n);
      std::cout << "Allocate "          << n
                << " block(s) of size " << sizeof(T)
                << " type \""           << typeid(T).name()
                << "\" at address "     << ptr << std::endl;
      if(ptr == nullptr)
        throw std::bad_alloc();
      return ptr;
    }

    void deallocate(void* ptr, size_type n) {
      if (ptr) {
        releaseBlock(static_cast<pointer>(ptr), n);
        std::cout << "Deallocate at address " << ptr << " n " << n << std::endl;
      }
    }

    pointer address(reference ref) const {
      return &ref;
    }

    const_pointer address(const_reference cref) const {
      return &cref;
    }

    CustomAllocator<T, N>&
    operator = (const CustomAllocator&) {
      std::cout << "operator = " << this << std::endl;
      return *this;
    }

    CustomAllocator<T, N>&
    operator = (const CustomAllocator&&) {
      std::cout << "operator = " << this << std::endl;
      return *this;
    }

    bool operator != (const CustomAllocator&) {
      return true;
    }

    bool operator == (const CustomAllocator&) {
      return false;
    }

    template<typename U, typename ...Args>
    void construct(U* ptr, Args &&...args) {
      new(ptr) U(std::forward<Args>(args)...);
    }

    void destroy(pointer ptr) {
      std::cout << "Destruct object at address " << ptr << std::endl;
      ptr->~T();
    }

    size_type max_size() const {
      return size_t(-1);
    }

    template <class U>
    struct rebind {
        typedef CustomAllocator<U, N> other;
    };

    template <class U>
    CustomAllocator(const CustomAllocator<U, N>&) {
      std::cout << "Template copy ctor allocator \""
                << typeid(T).name() << "\" " << "\""
                << typeid(U).name() << "\"" << std::endl;
    }

    template <class U>
    CustomAllocator& operator = (const CustomAllocator<U, N>&) {
      std::cout << "Template = " << this << std::endl;
      return *this;
    }

  private:

    std::array<uint8_t, N * sizeof(value_type)> data;
    std::array<bool, N> flags{{false}};

    pointer takeBlock(size_t n) {
      auto first = flags.begin();
      size_t cnt = 0;
      for(auto it = flags.begin(); it != flags.end(); ++it) {
        if(!(*it)) {
          if(cnt == 0)
            first = it;

          if(++cnt == n) {
            std::fill(first, ++it, true);
            auto pos = static_cast<size_t>(std::distance(flags.begin(), first));
            return reinterpret_cast<pointer>(&data[sizeof(value_type) * pos]);
          }
        }
        else
          cnt = 0;
      }
      return nullptr;
    }

    void releaseBlock(pointer ptr, size_t n) {
      int pos = ptr - reinterpret_cast<pointer>(&data[0]);
      if(pos >= 0 && pos + static_cast<int>(n) < static_cast<int>(N)) {
        auto first = flags.begin() + pos;
        auto last  = first + static_cast<int>(n);
        std::fill(first, last, false);
      }
    }
};

/**
 * @brief Частичная специализация шаблона аллокатора с произвольным количеством элементов
 * и с использованием кастомной кучи для их размещения.
 */
template <typename T>
class CustomAllocator<T, 0>
{
  public:
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    CustomAllocator() {
      std::cout << "Ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    ~CustomAllocator() {
      std::cout << "Dtor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    CustomAllocator(const CustomAllocator&) {
      std::cout << "Copy ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    pointer allocate(size_type n, const void* = 0) {
      T* ptr = reinterpret_cast<T*>(customMalloc(n * sizeof(T)));
      std::cout << "Allocate "          << n
                << " block(s) of size " << sizeof(T)
                << " type \""           << typeid(T).name()
                << "\" at address "     << ptr << std::endl;
      return ptr;
    }

    void deallocate(void* ptr, size_type n) {
      if (ptr) {
        customFree(ptr);
        std::cout << "Deallocate at address " << ptr << " n " << n << std::endl;
      }
    }

    pointer address(reference ref) const {
      return &ref;
    }

    const_pointer address(const_reference cref) const {
      return &cref;
    }

    CustomAllocator<T, 0>&
    operator = (const CustomAllocator&) {
      std::cout << "operator = " << this << std::endl;
      return *this;
    }

    CustomAllocator<T, 0>&
    operator = (const CustomAllocator&&) {
      std::cout << "operator = " << this << std::endl;
      return *this;
    }

    bool operator != (const CustomAllocator&) {
      return true;
    }

    bool operator == (const CustomAllocator&) {
      return false;
    }

    template<typename U, typename ...Args>
    void construct(U* ptr, Args &&...args) {
      new(ptr) U(std::forward<Args>(args)...);
    }

    void destroy(pointer ptr) {
      std::cout << "Destruct object at address " << ptr << std::endl;
      ptr->~T();
    }

    size_type max_size() const {
      return size_t(-1);
    }

    template <class U>
    struct rebind {
        typedef CustomAllocator<U, 0> other;
    };

    template <class U>
    CustomAllocator(const CustomAllocator<U, 0>&) {
      std::cout << "Template copy ctor allocator \""
                << typeid(T).name() << "\" " << "\""
                << typeid(U).name() << "\"" << std::endl;
    }

    template <class U>
    CustomAllocator& operator = (const CustomAllocator<U, 0>&) {
      std::cout << "Template = " << this << std::endl;
      return *this;
    }
};

int main()
{
  using cust_vec_alloc_t = CustomAllocator<int, 10>;
  using cust_vec_t = std::vector<int, cust_vec_alloc_t>;

  cust_vec_t vec1(3);

  for(int i = 0; i < 3; ++i)
    vec1[i] = i + 5;

  vec1.push_back(23);

  for(auto it: vec1)
    std::cout << it << std::endl;

  auto vec2 = vec1;

  vec1.pop_back();

  for(auto it: vec1)
    std::cout << it << std::endl;

  for(auto it: vec2)
    std::cout << it << std::endl;



  using cust_map_alloc_t = CustomAllocator<std::pair<const int, int>, 21>;
  using cust_map_t = std::map<int, int, std::less<int>, cust_map_alloc_t>;

  cust_map_t map1;

  for(int i = 0; i < 20; ++i)
    map1[i] = i;

  for(auto it: map1)
    std::cout << it.first << " " << it.second << std::endl;

  cust_map_t map2 = map1;

  for(auto it: map2)
    std::cout << it.first << " " << it.second << std::endl;

  return 0;
}
