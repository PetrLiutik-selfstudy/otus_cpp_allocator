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


    CustomVector() : data_(nullptr), size_(), capacity_() {}

    explicit CustomVector(size_type size) : size_(size), capacity_(size) {
      data_ = allocator_.allocate(size_);
      data_ = allocator_.construct(data_, size);
    }

    CustomVector(size_type size, T value) : size_(size), capacity_(size) {
      data_ = allocator_.allocate(size_);
      for(auto i = 0; i < size_; ++i)
          data_[i] = value;
    }

    CustomVector(const std::initializer_list<T>& vec) : size_(vec.size_), capacity_(vec.size_) {
      data_ = allocator_.allocate(size_);
      for(auto i = 0; i < vec.size(); ++i)
          data_[i] = *(vec.begin() + i);
    }

    CustomVector(const CustomVector<T>& vec) : size_(vec.size_), capacity_(vec.capacity_) {
      data_ = allocator_.allocate(capacity_);
      for(auto i = 0; i < size_; ++i)
          data_[i] = vec.data_[i];
    }

    CustomVector(CustomVector<T>&& vec) : size_(vec.size_), capacity_(vec.capacity_) {
      data_ = vec.data_;
      vec.data_ = nullptr;
      vec.size_ = 0;
      vec.capacity_ = 0;
    }

    CustomVector& operator = (const CustomVector<T>& vec) {
      if(this == &vec)
        return *this;

      for(auto i = 0; i < size_; i++)
        allocator_.destroy(&data_[i]);
      allocator_.deallocate(data_, capacity_);

      size_ = vec.size_;
      capacity_ = vec.capacity_;

      data_ = allocator_.allocate(capacity_);
      for(auto i = 0; i < size_; ++i)
          data_[i] = vec.data_[i];
    }

    CustomVector& operator = (CustomVector<T>&& vec) {
      if(this == &vec)
        return *this;

      for(auto i = 0; i < size_; ++i)
        allocator_.destroy(&data_[i]);
      allocator_.deallocate(data_, capacity_);

      size_ = vec.size_;
      capacity_ = vec.capacity_;
      data_ = vec.data_;
      vec.size_ = 0;
      vec.capacity = 0;
      vec.data_ = nullptr;

      return *this;
    }

    ~CustomVector() {
      for(auto i = 0; i < size_; ++i)
        allocator_.destroy(&data_[i]);
      allocator_.deallocate(data_, capacity_);
    }

    bool operator == (const CustomVector<T>& vec) {
      if(size_ == vec.size_ && capacity_ == vec.capacity_) {
        for(auto i = 0; i < size_; ++i)
          if(data_[i] != vec.data_[i])
            return false;
        return true;
      }
      return false;
    }

    bool operator != (const CustomVector<T>& vec) {
      return !operator == (vec);
    }

    size_type capacity() {
      return capacity_;
    }

    size_type size() {
      return size_;
    }

    void push_back(const T& item) {
      if(size_ == capacity_) {
        auto capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        auto data = allocator_.allocate(capacity);

        for(size_type i = 0; i < size_; ++i)
          data[i] = data_[i];
        data[size_] = item;

        for(size_type i = 0; i < size_; ++i)
          allocator_.destroy(&data_[i]);
        allocator_.deallocate(data_, capacity_);

        data_ = data;
        capacity_ = capacity;
        size_++;
      } else {
        data_[size_] = item;
        size_++;
      }
    }

    void pop_back() {
      if(size_ > 0) {
        size_--;
        allocator_.destroy(&data_[size_]);
      }
    }

    void swap(CustomVector& other) {
      auto data = other.data_;
      auto size = other.size_;
      auto capacity = other.capacity_;

      other.data_ = data_;
      other.size_ = size_;
      other.capacity_ = capacity_;

      size_ = size;
      capacity_ = capacity;
      data_ = data;
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

    void resize(size_type count) {
      if(count < size_)
        size_ = count;
      else if (count > capacity_) {
        auto capacity = count;
        auto data = allocator_.allocate(capacity);

        for(auto i = 0; i < size_; ++i)
          data[i] = data_[i];
        allocator_.construct(data_[size_], capacity - size_);

        for(auto i = 0; i < size_; ++i)
          allocator_.destroy(&data_[i]);
        allocator_.deallocate(data_, capacity_);

        data_ = data;
        capacity_ = capacity;
      }
    }

    void reserve(size_type count) {
      if(count > capacity_) {
        auto capacity = count;
        auto data = allocator_.allocate(capacity);

        for(auto i = 0; i < size_; ++i)
          data[i] = data_[i];

        for(auto i = 0; i < size_; ++i)
          allocator_.destroy(&data_[i]);
        allocator_.deallocate(data_, capacity_);

        data_ = data;
        capacity_ = capacity;
      }
    }

    struct iterator : std::iterator<std::bidirectional_iterator_tag, T> {
        explicit iterator(pointer current) : current_(current) {}

        iterator& operator ++() {
          current_++;
          return *this;
        }

        iterator& operator --() {
          current_--;
          return *this;
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

    iterator erase(iterator pos) {
      size_type i = 0;
      auto it = (*this).begin();
      for(it; it != pos; ++it, ++i)
        ;

      for(auto it = pos + 1; it != (*this).end(); ++it, ++i)
        data_[i] = data_[i + 1];

      size_--;
      return pos;
    }

    iterator erase(iterator first, iterator last) {
      size_type i = 0;
      size_type temp = 0;

      auto it = (*this).begin();
      for(it; it != first; ++it, ++i)
        ;
      for(it = first; it != last; ++it, ++temp, ++i)
        ;

      for(auto it = last; it != (*this).end(); ++it, ++i)
        data_[i - temp] = data_[i];

      size_ -= temp;
      return last;
    }

    iterator insert(iterator pos, const T & value) {
      size_type i = 0;
      if (capacity_ > size_){
        for(iterator it = data_ + size_; it != pos; --it, ++i)
          data_[size_ - i] = data_[size_ - i - 1];
        *pos = value;
        size_++;
      } else {
        pointer data = allocator_.allocate(size_ + 1);

        for(iterator it = data_; it != pos; it++, i++)
          data[i] = data_[i];
        data[i] = value;
        i++;
        for(iterator it = data_ + i + 1; it != data_ + size_ + 2; it++, i++)
          data[i] = data_[i - 1];

        for(auto i = 0; i < size_; ++i)
          allocator_.destroy(&data_[i]);
        allocator_.deallocate(data_, capacity_);

        data_ = data;
        size_++;
        capacity_ = size_;
      }
    }

    void insert(iterator pos, size_type count, const T& value) {
      pointer data = allocator_.allocate(size_ + count);
      size_type i = 0;
      size_type j = 0;

      for(iterator it = data_; it != pos; ++it, ++i)
        data[i] = data_[i];

      for(j = 0; j < count; j++)
        data[i + j] = value;

      for(iterator it = data_ + i; it != data_ + size_; ++it, ++i)
        data[i + j] = data_[i];

      for(auto i = 0; i < size_; ++i)
        allocator_.destroy(&data_[i]);
      allocator_.deallocate(data_, capacity_);

      size_ += count;
      capacity_ = size_;
      data_ = data;
    }

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

//    using alloc_t = typename allocator_type::template rebind<T>::other;
    allocator_type allocator_;
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


  CustomVector<int> custVec;
  custVec.push_back(10);

  for(auto it: custVec)
    std::cout << it << std::endl;

  return 0;
}
