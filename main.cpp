#include <iostream>
#include <map>
#include <vector>

using namespace std;

template <typename T>
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
      // TODO сделать собственный менеджер кучи.
      T* ptr = reinterpret_cast<T*>(malloc(n * sizeof(T)));
      std::cout << "Allocate "          << n
                << " block(s) of size " << sizeof(T)
                << " at address "       << ptr << std::endl;
      return ptr;
    }

    void deallocate(void* ptr, size_type) {
      if (ptr) {
        // TODO сделать собственный менеджер кучи.
        free(ptr);
        std::cout << "Deallocate at address " << ptr << std::endl;
      }
    }

    pointer address(reference ref) const {
      return &ref;
    }

    const_pointer address(const_reference cref) const {
      return &cref;
    }

    CustomAllocator<T>&
    operator = (const CustomAllocator&) {
      std::cout << "operator = " << this << std::endl;
      return *this;
    }

    void construct(pointer ptr, const T& val) {
      std::cout << "Construct object at address " << ptr << std::endl;
      new (reinterpret_cast<T*>(ptr)) T(val);
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
        typedef CustomAllocator<U> other;
    };

    template <class U>
    CustomAllocator(const CustomAllocator<U>&) {
      std::cout << "Template copy ctor allocator \""
                << typeid(T).name() << "\" " << "\""
                << typeid(U).name() << "\"" << std::endl;
    }

    template <class U>
    CustomAllocator& operator = (const CustomAllocator<U>&) {
      std::cout << "Template = " << this << std::endl;
      return *this;
    }
};

int main()
{
  std::map<int, int> aaa;
  aaa[1] = 21;

  std::vector<int> bbb;
  bbb.push_back(2);

  std::vector<int, CustomAllocator<int>> ccc;
  ccc.reserve(100);
  ccc.push_back(2);

  cout << "Hello World!" << endl;
  return 0;
}
