#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
// Non-standard implementation of the list data structure.
template <class T>
class list
{
public:
  typedef T value_type;

  // Custom iterator class
  class iterator
  {
  public:
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
    friend class list;

    iterator(T *ptr) : ptr_(ptr) {}

    T *ptr_;
  };

  list()
  {
    size_ = 0;
    data_ = nullptr;
  };

  // Constructs class with passed in element.
  list(T element)
  {
    size_ = 1;

    data_ = new T[size_];

    std::memcpy(data_, &element, sizeof(T));
  };

  // Constructs class with the passed in initializer_list.
  list(std::initializer_list<T> init_list)
  {
    size_ = init_list.size();

    data_ = new T[size_];

    std::memcpy(data_, std::begin(init_list), init_list.size() * sizeof(T));
  }

  // Returns the element at the front of the list.
  T &front() { return data_[0]; }

  // Returns the element at the back of the list.
  T &back()
  {
    if (size_ == 0)
    {
      return data_[0];
    }

    return data_[size_ - 1];
  }

  // Returns an iterator pointing at the front of the list.
  iterator begin() { return iterator(data_); }

  // Returns an iterator pointing at the back of the list.
  iterator end() { return iterator(data_ + size_); }

  // Inserts the passed in element at the passed in
  // iterator position.
  iterator insert(iterator position, const T &element)
  {
    int previous_size = size_;
    size_++;

    T *temp = new T[size_];

    // Counter tracks the index of the passed in iterator position.
    int counter = position.ptr_ - data_;

    int insert_index       = counter;
    int next_index         = counter + 1;
    int remaining_elements = previous_size - counter;

    // Copy the original data that comes before the insertion.
    std::memcpy(temp, data_, insert_index * sizeof(T));
    // Copy the inserted data.
    std::memcpy(temp + insert_index, &element, sizeof(T));
    // Copy the original data that comes after the insertion.
    std::memcpy(temp + next_index, data_ + counter, remaining_elements * sizeof(T));

    delete[] data_;

    data_ = temp;

    // Return an iterator at the inserted element.
    return iterator(data_ + counter);
  }

  // Insert an element at the end of the list.
  void push_back(T element)
  {

    int previous_size = size_;
    size_++;

    T *temp = new T[size_];

    // Copy the original data into the new array.
    for (int i = 0; i < previous_size; i++)
    {
      temp[i] = data_[i];
    }

    // Deallocate memory used by previous array.
    delete[] data_;

    data_ = temp;

    data_[previous_size] = element;
  };

  // Insert an element at the front of the list.
  void push_front(T element)
  {
    size_++;

    T *temp = new T[size_];

    // Copy the original data into the new array leaving space
    // for the new element at the front.
    for (unsigned int i = 1; i < size_; i++)
    {
      temp[i] = data_[i - 1];
    }

    // Deallocate memory used by previous array.
    delete[] data_;

    data_    = temp;
    data_[0] = element;
  };

  // Returns the last element and removes it from the list.
  T pop_back()
  {
    T back_elem = data_[size_ - 1];
    size_--;

    T *temp = new T[size_];

    // Copy the original data.
    std::memcpy(temp, data_, size_ * sizeof(T));

    // Deallocate memory used by previous array.
    delete[] data_;

    data_ = temp;

    return back_elem;
  }

  // Returns the first element and removes it from the list.
  T pop_front()
  {
    T front_elem = front();
    size_--;

    T *temp = new T[size_];

    // Copy the original data.
    std::memcpy(temp, data_ + 1, size_ * sizeof(T));

    // Deallocate memory used by previous array.
    delete[] data_;

    data_ = temp;

    return front_elem;
  }

  // Returns the number of elements in the list. //
  int size() { return size_; }

  list &operator=(const list &other_list)
  {
    delete[] data_;

    data_ = new T[other_list.size_];

    memcpy(data_, other_list.data_, other_list.size_ * sizeof(T));

    size_ = other_list.size_;

    return *this;
  }

  list(const list &other_list)
  {
    size_ = other_list.size_;

    data_ = new T[size_];

    memcpy(data_, other_list.data_, other_list.size_ * sizeof(T));
  }

  // Returns true if the list contents are the same, false if they are not
  bool operator==(const list &other_list)
  {
    if (size_ != other_list.size_)
    {
      return false;
    }
    
    return (memcmp(data_, other_list.data_, size_*sizeof(T)) == 0);
  }

  T &operator[](int index) { return data_[index]; }

  ~list() { delete[] data_; }

private:
  size_t size_;

  T *data_;
};
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
