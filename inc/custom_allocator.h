#pragma once

#include <iostream>
#include <array>
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
