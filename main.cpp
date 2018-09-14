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
#include <memory>

#include "custom_heap.h"

/**
 * @brief Шаблон аллокатора с параметрически заданным количеством элементов.
 */
template <typename T, size_t N = 0>
class CustomAllocator
{
  public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    CustomAllocator() {
      std::cout << "Ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    ~CustomAllocator() {
      std::cout << "Dtor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    CustomAllocator(const CustomAllocator& other) : data_(other.data_), flags_(other.flags_) {
      std::cout << "Copy ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
    }

    CustomAllocator(CustomAllocator&& other) noexcept {
      std::cout << "Move ctor allocator \"" << typeid(T).name() << "\"" << std::endl;
      other.swap(*this);
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

    CustomAllocator& operator = (CustomAllocator const& other) {
      std::cout << "operator = " << this << std::endl;
      CustomAllocator<T, N> tmp(other);
      tmp.swap(*this);
      return *this;
    }

    CustomAllocator& operator = (CustomAllocator&& other) noexcept {
      std::cout << "Move operator = " << this << std::endl;
      other.swap(*this);
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
      std::cout << "Construct object at address " << ptr << std::endl;
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
        using other = CustomAllocator<U, N>;
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

    std::array<uint8_t, N * sizeof(value_type)> data_;
    std::array<bool, N> flags_{{false}};

    pointer takeBlock(size_t n) {
      auto first = flags_.begin();
      size_t cnt = 0;
      for(auto it = flags_.begin(); it != flags_.end(); ++it) {
        if(!(*it)) {
          if(cnt == 0)
            first = it;

          if(++cnt == n) {
            std::fill(first, ++it, true);
            auto pos = static_cast<size_t>(std::distance(flags_.begin(), first));
            return reinterpret_cast<pointer>(&data_[sizeof(value_type) * pos]);
          }
        }
        else
          cnt = 0;
      }
      return nullptr;
    }

    void releaseBlock(pointer ptr, size_t n) {
      int pos = ptr - reinterpret_cast<pointer>(&data_[0]);
      if(pos >= 0 && pos + static_cast<int>(n) < static_cast<int>(N)) {
        auto first = flags_.begin() + pos;
        auto last  = first + static_cast<int>(n);
        std::fill(first, last, false);
      }
    }

    void swap(CustomAllocator& other) {
      std::swap(data_, other.data_);
      std::swap(flags_, other.flags_);
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
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

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
        using other = CustomAllocator<U, 0>;
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

template <typename T, typename A = std::allocator<T>>
class CustomVector {
  public:
    using size_type = size_t;
    using value_type = T;
    using allocator_type = A;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;

    CustomVector() : size_(0), capacity_(0), data_(nullptr) {
      allocator_ = std::make_unique<allocator_type>();
    }

    explicit CustomVector(size_type size) : size_(size), capacity_(size) {
      allocator_ = std::make_unique<allocator_type>();
      data_ = allocator_->allocate(size_);
      for(size_type i = 0; i < size_; ++i)
        allocator_->construct(&data_[i]);
    }

    CustomVector(size_type size, T value) : size_(size), capacity_(size) {
      allocator_ = std::make_unique<allocator_type>();
      data_ = allocator_->allocate(size_);
      for(size_type i = 0; i < size_; ++i)
        allocator_->construct(&data_[i], value);
    }

    CustomVector(const std::initializer_list<T>& vec) : size_(vec.size()), capacity_(vec.size()) {
      allocator_ = std::make_unique<allocator_type>();
      data_ = allocator_->allocate(size_);
      for(size_type i = 0; i < vec.size(); ++i)
        allocator_->construct(&data_[i], *(vec.begin() + i));
    }

    CustomVector(const CustomVector& vec) : size_(vec.size_), capacity_(vec.capacity_) {
      allocator_ = std::make_unique<allocator_type>();
      data_ = allocator_->allocate(capacity_);
      for(size_type i = 0; i < size_; ++i)
        allocator_->construct(&data_[i], vec.data_[i]);
    }

    CustomVector(CustomVector&& vec) noexcept :
      capacity_(0), size_(0), data_(nullptr) {
      vec.swap(*this);
    }

    CustomVector& operator = (CustomVector const& vec) {
      CustomVector<T, A> tmp(vec);
      tmp.swap(*this);
      return *this;
    }

    CustomVector& operator = (CustomVector&& vec) noexcept {
      vec.swap(*this);
      return *this;
    }

    ~CustomVector() {
      for(size_type i = 0; i < size_; ++i)
        allocator_->destroy(&data_[i]);
      allocator_->deallocate(data_, capacity_);
    }

    bool operator == (const CustomVector& vec) const {
      if(size_ != vec.size_ || capacity_ != vec.capacity_)
        return false;

      for(size_type i = 0; i < size_; i++)
        if (data_[i] != vec.data_[i])
          return false;

      return true;
    }

    bool operator != (const CustomVector& vec) const {
      return !operator == (vec);
    }

    size_type capacity() const {
      return capacity_;
    }

    size_type size() const {
      return size_;
    }

    void push_back(const T& value) {
      resizeIfRequire();
      pushBackInternal(value);
    }

    void pop_back() {
      if(size_ > 0) {
        size_--;
        allocator_->destroy(&data_[size_]);
      }
    }

    void swap(CustomVector& other) {
      std::swap(data_, other.data_);
      std::swap(size_, other.size_);
      std::swap(capacity_, other.capacity_);
      std::swap(allocator_, other.allocator_);
    }

    T& front() {
      if (size_ > 0)
        return data_[0];
      else
        throw std::out_of_range("Empty vector");
    }

    const T& front() const {
      if (size_ > 0)
        return data_[0];
      else
        throw std::out_of_range("Empty vector");
    }

    T& back() {
      if (size_ > 0)
        return data_[size_ - 1];
      else
        throw std::out_of_range("Empty vector");
    }

    const T& back() const {
      if (size_ > 0)
        return data_[size_ - 1];
      else
        throw std::out_of_range("Empty vector");
    }

    T& operator [] (size_type pos) {
      if (pos > size_ - 1 || pos < 0)
        throw std::out_of_range("Out of scope");
      else
        return data_[pos];
    }

    const T& operator [] (size_type pos) const {
      if (pos > size_ - 1 || pos < 0)
        throw std::out_of_range("Out of scope");
      else
        return data_[pos];
    }

    void resize(size_type size) {
      if(size < size_) {
        for(size_type i = size; i < size_; ++i)
          allocator_->destroy(&data_[i]);
        size_ = size;
      } else {
        size_type i = size_;
        if (size > capacity_)
          reserveCapacity(size);
        for(; i < size; ++i)
          allocator_->construct(&data_[size_]);
      }
    }

    void reserve(size_type capacity) {
      if(capacity > capacity_) {
        reserveCapacity(capacity);
      }
    }

    void clear() {
      for(size_type i = 0; i < size_; ++i)
        allocator_->destroy(&data_[i]);
      size_ = 0;
    }

    struct iterator : std::iterator<std::bidirectional_iterator_tag, T> {
        explicit iterator(pointer current) : current_(current) {}

        iterator& operator ++(){
          current_++;
          return *this;
        }

        iterator operator ++(int){
          iterator temp = *this;
          current_++;
          return temp;
        }

        iterator& operator --(){
          current_--;
          return *this;
        }

        iterator operator --(int){
          iterator temp = *this;
          current_--;
          return temp;
        }

        reference operator *() {
          return *current_;
        }

        bool operator == (iterator &other) {
          return current_ == other.current_;
        }

        bool operator != (iterator &other) {
          return !(*this == other);
        }

      private:
        pointer current_;
    };

    iterator end() {
      iterator it(data_ + size_);
      return it;
    }

    iterator begin() {
      iterator it(data_);
      return it;
    }

  private:
    size_type size_{0};
    size_type capacity_{0};
    pointer data_;
    std::unique_ptr<allocator_type> allocator_;

    void resizeIfRequire() {
      if (size_ == capacity_) {
        size_type newCapacity = capacity_ < 2 ? 2 : capacity_ * 2;
        reserveCapacity(newCapacity);
      }
    }

    void pushBackInternal(T const& value) {
      allocator_->construct(data_ + size_, value);
      ++size_;
    }

    void reserveCapacity(size_type newCapacity) {
      auto data = allocator_->allocate(newCapacity);

      for(size_type i = 0; i < size_; ++i) {
        allocator_->construct(&data[i], data_[i]);
        allocator_->destroy(&data_[i]);
      }
      allocator_->deallocate(data_, capacity_);

      data_ = data;
      capacity_ = newCapacity;
    }
};

constexpr uint32_t factorial(uint32_t n) {
  return n == 0 ? 1 : factorial(n - 1) * n;
}

int main()
{
  using cust_vec_alloc_t = CustomAllocator<uint32_t, 100>;
  using cust_vec_t = CustomVector<uint32_t, cust_vec_alloc_t>;

  cust_vec_t vec1{1, 2, 3};
  cust_vec_t vec2{5, 6, 7};
  vec1.clear();


  for(uint32_t i = 0; i < 10; ++i)
    vec1.push_back(factorial(i));

    vec1.pop_back();

  vec1.swap(vec2);
  cust_vec_t vec3(std::forward<cust_vec_t>(vec1));

  for(auto it: vec1)
    std::cout << it << std::endl;

  for(auto it: vec2)
    std::cout << it << std::endl;

  for(auto it: vec3)
    std::cout << it << std::endl;

//  using cust_map_alloc_t = CustomAllocator<std::pair<const int, int>, 21>;
//  using cust_map_t = std::map<int, int, std::less<int>, cust_map_alloc_t>;

//  cust_map_t map1;

//  for(int i = 0; i < 20; ++i)
//    map1[i] = i;

//  for(auto it: map1)
//    std::cout << it.first << " " << it.second << std::endl;

//  cust_map_t map2 = map1;

//  for(auto it: map2)
//    std::cout << it.first << " " << it.second << std::endl;

  return 0;
}
