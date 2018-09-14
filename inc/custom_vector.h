#pragma once

#include <iterator>
#include <algorithm>
#include <memory>

/**
 * @brief Шаблон кастомного вектора.
 */
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
      size_(0), capacity_(0), data_(nullptr) {
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