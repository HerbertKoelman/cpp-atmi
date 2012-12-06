#include "ThreadPool.h"
#include <sstream>
#include <iostream>

/*
   ThreadPool constructor.
   Initialize local variables (ie min and max workers).
   Create the permanent workers and start them.
 */
ThreadPool::ThreadPool(unsigned int min, unsigned int max) {
  this->number_of_permanent_workers = min;
  this->max_concurrent_workers = max;
  this->workers_list.resize(this->number_of_permanent_workers);

  for(unsigned int i = 0; i < this->number_of_permanent_workers; i++) {
    std::stringstream ss;
    ss << i;
    this->workers_list[i] = new Worker(this, "Worker " + ss.str());
    this->workers_list[i]->start();
  }
}

/*
   ThreadPool destructor.
   Stop each permanent worker and wait for the barrier to signal the terminaison of every worker.
 */
ThreadPool::~ThreadPool() {
  //signal terminaison to each permanent worker
  for(unsigned int i = 0; i < this->number_of_permanent_workers; i++) {
    this->workers_list[i]->stop();
  }
  //waiting than every threads have done their job.
  while(b_cond.get_value() > 0) b_cond.wait();
  //delete worker
  for(unsigned int i = 0; i < this->number_of_permanent_workers; i++) {
    delete this->workers_list[i];
  }
}

/*
   This function add a worker to the waiting list. A waiting worker
   will be used for next job.
 */
void ThreadPool::add_worker_to_waiting_list(Worker *w) {
  ScopedMutex lock(update_cond);
  //check if worker is already in the list
  for(std::list<Worker *>::iterator it = waiting_workers.begin(); it != waiting_workers.end(); ++it) {
    if(*it == w) {
      return;
    }
  }
  waiting_workers.push_back(w);
  //signal the arrival of the worker
  update_cond.signal();
}

/*
   This function return the first permanent worker waiting in the waiting list.
   If no permanent worker is available create a temporary worker.
   If max worker is reached returns no worker.
 */
Worker * ThreadPool::get_worker() {
  Worker *first_waiting_worker;
  ScopedMutex lock(update_cond);
  if(!this->waiting_workers.empty()) {
    //Permanent worker available
    //std::cout << "Waiting worker found." << std::endl;
    first_waiting_worker = this->waiting_workers.front();
    this->waiting_workers.pop_front();

  } else {
    //No permanent worker availaible
    //std::cout << "No waiting worker found." << std::endl;
    if(this->b_cond.get_value() < this->max_concurrent_workers) {
      //creating new worker
      //std::cout << "Creating temporary worker." << std::endl;
      first_waiting_worker = new Worker(this, "Temporary worker.", 1);
      first_waiting_worker->start();

    } else {
      //waiting for a worker
      //std::cout << "Waiting for worker." << std::endl;
      /*while(this->waiting_workers.empty()) {
          update_cond.simple_wait();
         }
         if(!this->waiting_workers.empty()) {
          first_waiting_worker = this->waiting_workers.front();
          this->waiting_workers.pop_front();
         }*/
      //No available worker
      //std::cout << "No worker found." << std::endl;
      first_waiting_worker = (Worker *)0;
    }
  }
  return first_waiting_worker;
}

/*
   Signal the threadpool that a job has to be done.
   The thread pool will retrieve a worker if it can and do the job or
   return an error if no available worker can be found.
 */
int ThreadPool::do_job(Runnable *job) {
  //get a worker
  Worker *w = get_worker();
  if(w) {
    //std::cout << "Assign job to worker." << std::endl;
    w->set_job(job);
  } else {
    //std::cout << "No available worker." << std::endl;
    return -1;
  }
  return 0;
}

/*
   This function add a worker to the barrier
 */
void ThreadPool::barrier_add_workers() {
  this->b_cond.up();
}

/*
   This function remove a worker from the barrier
 */
void ThreadPool::barrier_remove_workers() {
  this->b_cond.down();
}

/*
   Barrier constructor.
   The number of worker is initialized  to zero.
 */
Barrier::Barrier() : n(0) {
}

/*Barrier destructor.
 */
Barrier::~Barrier() {
}

/*
   Increment the barrier
 */
void Barrier::up() {
  ScopedMutex lock(term_cond);
  this->n++;
  //std::cout << "n workers : " << this->n << ".\n";
}

/*
   decrease the barrier.
   if value is zero send a signal the barrier has been desactivated.
 */
void Barrier::down() {
  ScopedMutex lock(term_cond);
  this->n--;
  //std::cout << "n workers : " << this->n << ".\n";
  if(this->n <= 0) term_cond.broadcast();
}

/*
   Wait function.
   This wait that the barrier is desactivated.
 */
void Barrier::wait() {
  ScopedMutex lock(term_cond);
  //waiting condition
  if(this->n >0) while(this->n > 0) term_cond.simple_wait();
}
