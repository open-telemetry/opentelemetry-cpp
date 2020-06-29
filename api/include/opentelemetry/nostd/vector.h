#pragma once

#include <cstring>
#include <iostream>

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
// Non-standard implementation of the list data structure.
template <class T>
class vector
{
public:
  typedef T value_type;

  // Custom iterator class
  class iterator
  {
  public:
    typedef T value_type;
    typedef std::forward_iterator_tag iterator_category;
    typedef iterator self_type;
    typedef T &reference;
    typedef T *pointer;
    typedef int difference_type;

    iterator operator++()
    {
      ptr_++;
      return ptr_;
    }

    T &operator*() { return *ptr_; }

    T *operator->() { return ptr_; }

    bool operator==(const iterator &other) { return ptr_ == other.ptr_; }

    bool operator!=(const iterator &other) { return ptr_ != other.ptr_; }

  private:
    friend class vector;

    iterator(T *ptr) : ptr_(ptr) {}

    T *ptr_;
  };

  // Custom const iterator class
  class const_iterator
  {
  public:
    typedef std::forward_iterator_tag iterator_category;
    typedef const_iterator self_type;
    typedef T value_type;
    typedef T &reference;
    typedef T *pointer;
    typedef int difference_type;

    const_iterator operator++()
    {
      ptr_++;
      return ptr_;
    }

    const T &operator*() { return *ptr_; }

    const T *operator->() { return ptr_; }

    bool operator==(const const_iterator &other) { return ptr_ == other.ptr_; }

    bool operator!=(const const_iterator &other) { return ptr_ != other.ptr_; }

  private:
    friend class vector;

    const_iterator(T *ptr) : ptr_(ptr) {}

    T *ptr_;
  };

  vector() : size_(0), capacity_(0), data_(nullptr){};

  // Constructs class with passed in element.
  vector(T element) : size_(1), capacity_(1)
  {
    data_ = new T[capacity_];

    data_[0] = element;
  };

  // Constructs class with the passed in initializer_list.
  vector(std::initializer_list<T> init_list)
  {
    size_     = init_list.size();
    capacity_ = size_ + 1;
    data_     = new T[capacity_];

    // Copy initializer_list contents into data_
    std::copy(init_list.begin(), init_list.end(), begin());
  }

  // Returns the element at the front of the list.
  T &front() { return data_[0]; }

  // Returns the element at the back of the list.
  T &back() { return data_[size_ - 1]; }

  // Returns an iterator pointing at the front of the list.
  iterator begin() { return iterator(data_); }

  // Returns an iterator pointing at the back of the list.
  iterator end() { return iterator(data_ + size_); }

  // Returns a const iterator pointing at the front of the list.
  const_iterator begin() const { return const_iterator(data_); }

  // Returns a const iterator pointing at the back of the list.
  const_iterator end() const { return const_iterator(data_ + size_); }

  // Inserts the passed in element at the passed in
  // iterator position.
  iterator insert(iterator position, const T &element)
  {
    int previous_size = size_;
    size_++;

    // Counter tracks the index of the passed in iterator position.
    int counter = position.ptr_ - data_;

    int insert_index       = counter;
    int next_index         = counter + 1;
    int remaining_elements = previous_size - counter;

    if (size_ >= capacity_)
    {
      resize();
    }

    // Save the last element in the array to avoid overwriting it
    T last_element = data_[next_index + remaining_elements - 1];

    // Shift the elements to make room for the insertion
    for (int i = (next_index + remaining_elements - 1); i >= next_index; i--)
    {
      data_[i] = data_[i - 1];
    }

    data_[next_index + remaining_elements] = last_element;

    data_[insert_index] = element;

    // Return an iterator at the inserted element.
    return iterator(data_ + counter);
  }

  // Insert an element at the end of the list.
  void push_back(T element)
  {

    int previous_size = size_;
    size_++;
    if (size_ >= capacity_)
    {
      resize();
    }

    data_[previous_size] = element;
  };

  // Returns the last element and removes it from the list.
  T pop_back()
  {
    T back_elem = data_[size_ - 1];
    size_--;

    if (size_ < (capacity_ / 2))
    {
      resize();
    }

    return back_elem;
  }

  // Resizes the data_ array to be 1.25 times the size of the data
  // and sets the capacity_.
  size_t resize()
  {
    if (size_ < 4)
    {
      capacity_ = 4;
    }
    else
    {

      // Set the new capacity to twice the size.
      size_t new_capacity = size_ * 2;

      capacity_ = new_capacity;
    }

    T *temp = new T[capacity_];

    std::memcpy(temp, data_, (size_ - 1) * sizeof(T));

    delete[] data_;

    data_ = temp;

    return capacity_;
  }

  // Returns the number of elements in the list.
  int size() { return size_; }

  vector &operator=(const vector &other_vector)
  {
    capacity_ = other_vector.capacity_;
    size_     = other_vector.size_;

    delete[] data_;

    data_ = new T[capacity_];

    std::copy(other_vector.begin(), other_vector.end(), begin());

    return *this;
  }

  vector(const vector &other_vector)
  {
    size_ = other_vector.size_;

    capacity_ = other_vector.capacity_;

    data_ = new T[capacity_];

    std::copy(other_vector.begin(), other_vector.end(), begin());
  }

  // Returns true if the list contents are the same, false if they are not
  bool operator==(const vector &other_vector)
  {
    if (size_ != other_vector.size_)
    {
      return false;
    }

    return std::equal(begin(), end(), other_vector.begin());
  }

  T &operator[](size_t index) { return data_[index]; }

  ~vector() { delete[] data_; }

private:
  size_t size_;
  size_t capacity_;
  T *data_;
};

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
