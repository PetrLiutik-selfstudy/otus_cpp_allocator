#include <iostream>
#include <map>
#include <vector>
#include <array>

#include "custom_heap.h"

using namespace std;

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
//      pointer ptr = reinterpret_cast<pointer>(customMalloc(n * sizeof(T)));
//      auto ptr = takeItem(n);
      pointer ptr = reinterpret_cast<pointer>(aaa);
      std::cout << "Allocate "          << n
                << " block(s) of size " << sizeof(T)
                << " type \""           << typeid(T).name()
                << "\" at address "     << ptr << std::endl;
      std::cout << "N "          << N << std::endl;
      if(ptr == nullptr)
        throw bad_alloc();
      return ptr;
    }

    void deallocate(void* ptr, size_type n) {
      if (ptr) {
//        customFree(ptr);
//        releaseItem(static_cast<pointer>(ptr), n);
        std::cout << "Deallocate at address " << ptr << std::endl;
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

    uint8_t aaa[sizeof(T)];
//    T aaa;

//    struct item_t {
//      uint8_t data[sizeof(value_type)];
//      bool    isBusy{false};
//      uint8_t pad[3];
//    };

//    std::array<item_t, N> items;

//    pointer takeItem(size_t n) {
////      auto firstItem = items.begin();
////      size_t cnt = 0;
////      for(auto it = items.begin(); it != items.end(); ++it) {
////        if(!it->isBusy) {
////          if(cnt == 0)
////            firstItem = it;

////          if(++cnt == n) {
////            for(auto it1 = firstItem; it1 != items.end() && cnt > 0; ++it1) {
////              it1->isBusy = true;
////              cnt--;
////            }
////            return reinterpret_cast<pointer>(firstItem->data);
////          }
////        }
////        else {
////          cnt = 0;
////        }
////      }
////      return nullptr;

//      size_t firstItem = 0;
//      size_t cnt = 0;
//      for(size_t i = 0; i < N; ++i)
//      {
//        if(!items[i].isBusy)
//        {
////          if(cnt == 0)
////            firstItem = i;

////          if(++cnt == n)
////          {
////            for(size_t j = firstItem; j < N && cnt > 0; ++j)
////            {
//              items[i].isBusy = true;
////              cnt--;
////            }
//              pointer ptr = reinterpret_cast<pointer>(&items[firstItem].data[0]);
//            return ptr;
////          }


//        }
//      }
//      return nullptr;
//    }

//    void releaseItem(pointer ptr, size_t n) {
//      for(auto& it: items) {
//        if(reinterpret_cast<pointer>(it.data) == ptr) {
//          it.isBusy = false;
////          if(--n == 0)
////            break;
//        }
//      }
//    }
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
      // TODO сделать собственный менеджер кучи.
      T* ptr = reinterpret_cast<T*>(customMalloc(n * sizeof(T)));
      std::cout << "Allocate "          << n
                << " block(s) of size " << sizeof(T)
                << " type \""           << typeid(T).name()
                << "\" at address "     << ptr << std::endl;
      return ptr;
    }

    void deallocate(void* ptr, size_type n) {
      if (ptr) {
        // TODO сделать собственный менеджер кучи.
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
//  int* xxx = NULL;
//  volatile size_t freeHeapSize = getFreeHeapSize();


//  xxx = reinterpret_cast<int*>(customMalloc((1000 * sizeof(int))));

//  freeHeapSize = getFreeHeapSize();

//  customFree(xxx);

//  freeHeapSize = getFreeHeapSize();

//  std::map<int, int> aaa;
//  aaa[1] = 21;

//  std::vector<int> bbb;
//  bbb.push_back(2);

//  freeHeapSize = getFreeHeapSize();

  std::vector<int, CustomAllocator<int, 1>> ccc(1);
//  ccc.push_back(2);
//  ccc.reserve(100);
//  ccc.push_back(2);

//  freeHeapSize = getFreeHeapSize();

  cout << "Hello World!" << endl;
  return 0;
}
