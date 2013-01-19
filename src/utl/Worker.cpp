#include "ThreadPool.h"
#include <iostream>
#include <time.h>
#include <unistd.h>

namespace atmi {

  /*
     Constructor
     Create the worker with no job.
     pool is a ponter to the threadpool owning the worker.
     You can give a name to the worker.
     And finally add the worker in the barrier.
   */
  Worker::Worker(ThreadPool *tp, std::string surname, int destroy) : Thread(destroy) {
    this->pool = tp;
    this->Job = (Runnable *)0;
    this->end_job = false;
    this->surname = surname;
    //add worker to the barrier
    this->pool->barrier_add_workers();
  }

  /*
     Destructor.
   */
  Worker::~Worker() {
  }

  /*
     Main function of the worker. Once a worker is initialized, you enter this function. It basically consist of waiting a job.
     Once a job is recieved, the worker will do it and will signal to the thread pool when the job is finnished.
   */
  void Worker::run() {
    //std::cout << this->surname + " -- Starting thread." << std::endl;
    while(!end_job) {
      //checking starting condition
      if(!destroy_when_ended()) {
        //The worker is a permanent worker and will be added to the waiting list of the threadpool.
        //std::cout << this->surname + " -- Adding to wainting list." << std::endl;
        pool->add_worker_to_waiting_list(this);
      }
      //Wait that a job is given to the worker.
      {
        ScopedMutex lock(s_cond);
        while(this->Job == (Runnable *)0 && !end_job) s_cond.simple_wait();
      }
      if(this->Job != (Runnable *)0) {
        //std::cout << this->surname + " -- Do the Job." << std::endl;
        //Do the job.
        this->Job->run();
        ScopedMutex lock(s_cond);
        Job = (Runnable *)0;
        if(destroy_when_ended())
        {
          //This worker is a temporary worker and should be destroyed after execution of the job.
          //std::cout << this->surname + " -- Terminate temporary job." << std::endl;
          this->end_job = true;
        }
      }
    }
    //std::cout << this->surname + " -- Ending thread.\n";
    //remove worker to the barrier
    this->pool->barrier_remove_workers();
  }

  /*
     This function allow to stop a permanent worker by signaling the end condition.
   */
  void Worker::stop() {
    ScopedMutex lock(s_cond);
    //std::cout << this->surname + " -- Ending job." << std::endl;
    this->end_job = true;
    //signal the end
    s_cond.signal();
  }

  /*
     This function assign a job to the worker and signal that a job should be done.
   */
  void Worker::set_job(Runnable *job) {
    if(this->Job == (Runnable *)0) {
      ScopedMutex lock(s_cond);
      //std::cout << this->surname + " -- Signaling job." << std::endl;
      this->Job = job;
      //signal the job
      s_cond.signal();
    }
  }

  TestJob::TestJob(std::string msg, int time) {
    this->msg = msg;
    this->sleeptime = time;
  }

  TestJob::~TestJob() {
  }

  void TestJob::run() {
    std::cout << msg << std::endl;
    sleep(this->sleeptime);
  }
}
