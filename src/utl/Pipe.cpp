#include "Concurrency.h"

namespace atmi {

  /*
     Pipe constructor.
   */
  template<class T>
  Pipe<T>::Pipe() {
  }

  /*
     Pipe destructor
   */
  template<class T>
  Pipe<T>::~Pipe() {
  }

  /*
     add a message to the queue.
   */
  template <class T>
  void Pipe<T>::push(T *msg)
  {
    //add msg to the queue
    ScopedMutex lock(this->c_pipe);
    this->msg_list.push_back(msg);
    //signal the arrival of the msg
    this->c_pipe.signal();
  }

  /*
     Get first message in the queue and remove it.
   */
  template <class T>
  T* Pipe<T>::pop()
  {
    //get the first msg in the queue
    ScopedMutex lock(this->c_pipe);
    if(this->msg_list.size() <= 0) {
      //wait for a message to arrive
      while(this->msg_list.size() <= 0) this->c_pipe.simple_wait();
    }
    //get message
    T* msg = this->msg_list.front();
    //delete mesg from queue
    this->msg_list.pop_front();

    return msg;
  }

  /*
     Get number of message waiting in the queue.
   */
  template <class T>
  int Pipe<T>::size()
  {
    ScopedMutex lock(this->c_pipe);
    return this->msg_list.size();
  }
}
