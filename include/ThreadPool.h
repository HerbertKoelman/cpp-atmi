/*
 *  $Id$
 *
 * Class for dealing with thread pool.
 *
 * Eric BELLONI
 */

#ifndef ATMIPLUSPLUS_THREADPOOL_H
#define ATMIPLUSPLUS_THREADPOOL_H

#include <vector>
#include <list>
#include "Thread.h"
#include "Concurrency.h"

/**
 * Class barrier.
 * This class allow to create a barrier on a value wich can be increased or decreased.
 * We can wait the desactivation of the barrier. This desactivation is signal when the value has reached zero.
 */
class Barrier {
  private:
    /**
     * Barrier
     */
    unsigned int n;
    Condition term_cond;
  public:
    /**
     * constructor and destructor.
     */
    Barrier();
    ~Barrier();

    /**
     * increase the value.
     */
    void up();
    /**
     * decrease the value
     */
    void down();
    /**
     * wait for the desactivation of the barrier.
     */
    void wait();

    /**
     * get the current size of the barrier.
     *
     * @return the size of the barrier
     */
    unsigned int get_value() const {
      return n;
    }
};

//forward declaration of worker
class Worker;

/**
 * This class define a threadpool owning several permanent worker and can create temporary workers when it is saturated.
 * It keeps track of the permanent worker in a waiting list. Getting one of them if a job arrives.
 * If no permanent worker is found, the threadpool createss one temporary worker to do it if the maximum
 * allowed wqorker has not been reached.
 */
class ThreadPool {

  protected:
    /**
     * max thread in thread pool
     */
    unsigned int number_of_permanent_workers;
    unsigned int max_concurrent_workers;
    /**
     * list of threads
     */
    std::vector<Worker *> workers_list;

    /**
     * waiting thread
     */
    std::list<Worker *> waiting_workers;
    /**
     * mutex to update waiting thread
     */
    Condition update_cond;

    /**
     * get handle on a waiting thread
     *
     * @return handle on a waiting worker.
     */
    Worker * get_worker();

    //Barrier
    Barrier b_cond;
  public:
    /**
     * constructor
     */
    ThreadPool(unsigned int, unsigned int);
    /**
     * destructor
     */
    ~ThreadPool();
    /**
     * accessors
     *
     * @return number of maximum worker in threadpool.
     */
    unsigned int get_max_workers() const {
      return this->max_concurrent_workers;
    }
    /**
     * accessors
     *
     * @return current number of permanent worker.
     */
    unsigned int get_permanent_workers() const {
      return this->number_of_permanent_workers;
    }
    /**
     * accessors
     *
     * @return number of working workers.
     */
    unsigned int get_current_workers() const {
      return b_cond.get_value();
    }

    /**
     * add a worker to the waiting list
     */
    void add_worker_to_waiting_list(Worker *);

    /**
     * do job
     */
    int do_job(Runnable *);

    /**
     * barrier management
     */
    void barrier_add_workers();
    void barrier_remove_workers();
};

/**
 * This is a test class to test the threadpool.
 */
class TestJob : public Runnable {
  protected:
    std::string msg;
    int sleeptime;

  public:
    TestJob(std::string, int);
    ~TestJob();
    virtual void run();
};

/**
 * This class define a worker. A worker is waiting for a job to arrive. When one job is assigned to the worker.
 * The job is done. Once the job has been done, the worker add himself to the waiting list of the threadpool or, if
 * it is a temporary worker, suicide to free the memory.
 */
class Worker : public Thread {
  protected:
    std::string surname;
    /**
     * job assigne to the worker.
     */
    Runnable *Job;

    /**
     * ThreadPool owning this worker;
     */
    ThreadPool *pool;
    /**
     * starting condition
     */
    Condition s_cond;

    /**
     * ending condition
     */
    bool end_job;

  public:
    /**
     * constructors
     *
     * @param tp handle on threadpool owning the worker
     * @param surname name of worker.
     * @param destroy 1 if temporary worker 0 if permanent worker.
     */
    Worker(ThreadPool *tp, std::string surname = "", int destroy = 0);
/**
 * destructor
 */
    ~Worker();

    /**
     * accessors
     *
     * @return true if worker is working, false if worker is not working.
     */
    bool is_working() const {
      return (this->Job != (Runnable *)0);
    }

    /**
     * Main function call when the thread is starting
     */
    virtual void run();

    /**
     * stop the execution of a permanent  worker
     */
    void stop();
    /**
     * assign a job to the worker
     *
     * @param job job to assign to the worker.
     */
    void set_job(Runnable *);
};

#endif //ATMIPLUSPLUS_THREADPOOL_H
