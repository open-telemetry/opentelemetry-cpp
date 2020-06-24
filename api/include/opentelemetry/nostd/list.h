#pragma once

#include <array>
#include <memory>
#include <cstring>
#include <algorithm>
#include <iterator>

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
  /* Non-standard implementation of the list data structure. */
  template <class T>
    class list {
      public:

        typedef T value_type;
        
        /* Custom iterator class*/
        class iterator 
        {
          public:
            typedef std::forward_iterator_tag iterator_category;
            iterator(T* ptr) : ptr_(ptr) {}
            iterator operator++() {ptr_++; return ptr_; }
            T& operator*() { return *ptr_; }
            T* operator->() { return ptr_; }
            bool operator==(const iterator& other) {return ptr_ == other.ptr_; }
            bool operator!=(const iterator& other) {return ptr_ != other.ptr_; }
          
          private:
            T* ptr_;
        };
        
        list(){
          size_ = 0;
          data_ = NULL; 
        };
        
        /* Constructs class with passed in element. */
        list(T element){
          size_ = 1;
           
          T * data_ = new T [size_];

          std::memcpy(data_, &element, sizeof(T)); 
        };
        
        /* Constructs class with the passed in initializer_list. */ 
        list(std::initializer_list<T> init_list){
          size_ = init_list.size();

          T * temp = new T[size_];

          std::memcpy(temp, std::begin(init_list), init_list.size()*sizeof(T)); 

          data_ = temp;
        }
        
        /* Returns the element at the front of the list. */
        T front(){
          if(size_ == 0){
            return T();
          }
          return data_[0];
        }
        
        /* Returns the element at the back of the list. */
        T back(){
          if(size_ == 0){
            return T();
          }
          return data_[size_ - 1];
        }
        
        /* Returns an iterator pointing at the front of the list. */
        iterator begin(){
          return iterator(data_);
        }

        /* Returns an iterator pointing at the back of the list. */
        iterator end(){
          return iterator(data_ + size_);
        }

        /* Inserts the passed in element at the passed in 
         * iterator position.
         */
        iterator insert(iterator position, const T& element){
          int previous_size = size_; 
          size_++;
          T *temp = new T[size_];
          
          /* Counter tracks the index of the passed in iterator position. */
          int counter = 0;
          for(auto iter = begin(); iter != end(); ++iter){
            if(iter == position){
              break;
            }
            counter++;
          } 
          
          int insert_index = counter;
          int next_index = counter+1;
          int last_index = previous_size;
          int remaining_elements = previous_size - counter;
          
          /* Copy the original data that comes before the insertion. */
          std::memcpy(temp, data_, insert_index*sizeof(T));
          /* Copy the inserted data. */
          std::memcpy(temp + insert_index, &element, sizeof(T));
          /* Copy the original data that comes after the insertion. */
          std::memcpy(temp + next_index, data_+counter , remaining_elements*sizeof(T));
          
          T *old_data = data_;

          data_ = temp; 
          
          auto iter_pos = begin();
          /* Iterate until the inserted element. */
          for(int i = 0; i < insert_index; i++){
            ++iter_pos;
          }
          
          /* Deallocate memory used by previous array. */  
          delete[] old_data;
          
          /* Return an iterator at the inserted element. */ 
          return iter_pos; 
        }
        
        /* Insert an element at the end of the list. */
        void push_back(T element){
          int previous_size = size_;
          size_++;

          T *temp = new T[size_];
          
          std::memcpy(temp, data_, previous_size*sizeof(T)); 

          /* Save old array pointer for later deallocation. */ 
          T *old_data = data_;

          data_ = temp;

          data_[previous_size] = element;
          
          /* Deallocate memory used by previous array. */  
          delete[] old_data;
        };

        /* Insert an element at the front of the list. */
        void push_front(T element){
          int previous_size = size_;
          size_++;

          T *temp = new T[size_];

          /* Copy the original data, leaving space for one element
           * at the front. */
          std::memcpy(temp + 1, data_, previous_size*sizeof(T)); 

          /* Save old array pointer for later deallocation. */ 
          T *old_data = data_;

          data_ = temp;

          data_[0] = element;
          
          /* Deallocate memory used by previous array. */  
          delete[] old_data;
        };

        /* Returns the last element and removes it from the list. */ 
        T pop_back(){
          T back_elem = data_[size_ - 1];
          size_--;
          
          T *temp = new T[size_];
          
          /* Copy the original data.*/
          std::memcpy(temp, data_, size_*sizeof(T)); 
          
          /* Save old array pointer for later deallocation. */ 
          T *old_data = data_;
          
          data_ = temp;

          /* Deallocate memory used by previous array. */  
          delete[] old_data;
          
          return back_elem;
        }
        
        /* Returns the first element and removes it from the list. */ 
        T pop_front(){
          T front_elem = front();
          size_--;
          
          T *temp = new T[size_];
          
          /* Copy the original data.*/
          std::memcpy(temp, data_ + 1, size_*sizeof(T)); 
          
          /* Save old array pointer for later deallocation. */ 
          T *old_data = data_;
          
          data_ = temp;

          /* Deallocate memory used by previous array. */  
          delete[] old_data;
          
          return front_elem;
        }

        /* Returns the number of elements in the list. */
        int size(){
          return size_;
        }

        void operator=(const list& other_list){
          data_ = other_list.data_;
          size_ = other_list.size_;
        }
        
        T& operator[](int index){
          return data_[index];
        }

      private:
        T* data_;
        size_t size_; 
    };

}
OPENTELEMETRY_END_NAMESPACE


