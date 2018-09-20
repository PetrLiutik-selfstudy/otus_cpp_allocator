#pragma once

#include <algorithm>
#include <array>

#include "custom_heap.h"

namespace custom {
/**
 * @brief Шаблон аллокатора с параметрически заданным количеством элементов.
 */
template <typename T, size_t N = 0>
class allocator
{
  public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    allocator() {}

    ~allocator() {}

    allocator(const allocator& other) : data_(other.data_), flags_(other.flags_) {}

    allocator(allocator&& other) noexcept {
      other.swap(*this);
    }

    pointer allocate(size_type n, const void* = 0) {
      auto ptr = takeBlock(n);
      if(ptr == nullptr)
        throw std::bad_alloc();
      return ptr;
    }

    void deallocate(void* ptr, size_type n) {
      if (ptr) {
        releaseBlock(static_cast<pointer>(ptr), n);
      }
    }

    pointer address(reference ref) const {
      return &ref;
    }

    const_pointer address(const_reference cref) const {
      return &cref;
    }

    allocator& operator = (allocator const& other) {
      allocator<T, N> tmp(other);
      tmp.swap(*this);
      return *this;
    }

    allocator& operator = (allocator&& other) noexcept {
      other.swap(*this);
      return *this;
    }

    bool operator != (const allocator&) {
      return true;
    }

    bool operator == (const allocator&) {
      return false;
    }

    template<typename U, typename ...Args>
    void construct(U* ptr, Args &&...args) {
      new(ptr) U(std::forward<Args>(args)...);
    }

    void destroy(pointer ptr) {
      ptr->~T();
    }

    size_type max_size() const {
      return size_t(-1);
    }

    template <class U>
    struct rebind {
        using other = allocator<U, N>;
    };

    template <class U>
    allocator(const allocator<U, N>&) {}

    template <class U>
    allocator& operator = (const allocator<U, N>&) {
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

    void swap(allocator& other) {
      std::swap(data_, other.data_);
      std::swap(flags_, other.flags_);
    }
};

/**
 * @brief Частичная специализация шаблона аллокатора с произвольным количеством элементов
 * и с использованием кастомной кучи для их размещения.
 */
template <typename T>
class allocator<T, 0>
{
  public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    allocator() {}

    ~allocator() {}

    allocator(const allocator&) {}

    allocator(const allocator&&) {}

    pointer allocate(size_type n, const void* = 0) {
      T* ptr = reinterpret_cast<T*>(custom::malloc(n * sizeof(T)));
      return ptr;
    }

    void deallocate(void* ptr, size_type) {
      if (ptr) {
        custom::free(ptr);
      }
    }

    pointer address(reference ref) const {
      return &ref;
    }

    const_pointer address(const_reference cref) const {
      return &cref;
    }

    allocator<T, 0>&
    operator = (const allocator&) {
      return *this;
    }

    allocator<T, 0>&
    operator = (const allocator&&) {
      return *this;
    }

    bool operator != (const allocator&) {
      return true;
    }

    bool operator == (const allocator&) {
      return false;
    }

    template<typename U, typename ...Args>
    void construct(U* ptr, Args &&...args) {
      new(ptr) U(std::forward<Args>(args)...);
    }

    void destroy(pointer ptr) {
      ptr->~T();
    }

    size_type max_size() const {
      return size_t(-1);
    }

    template <class U>
    struct rebind {
        using other = allocator<U, 0>;
    };

    template <class U>
    allocator(const allocator<U, 0>&) {}

    template <class U>
    allocator& operator = (const allocator<U, 0>&) {
      return *this;
    }
};

}
