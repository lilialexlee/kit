/*
 * thread_pool.cc
 *
 */

#include "util/thread_pool.h"
#include "util/log.h"

namespace kit {

ThreadPool::ThreadPool()
    : tasks_(),
      max_task_num_(0),
      run_(false),
      threads_(),
      mutex_(),
      cond_() {
}

ThreadPool::~ThreadPool() {
  Stop();
}

void ThreadPool::Init(unsigned int max_tasks, int thread_num) {
  max_task_num_ = max_tasks;
  run_ = true;
  for (int i = 0; i < thread_num; ++i) {
    ThreadPtr thread(new boost::thread(boost::bind(&ThreadPool::Run, this)));
    threads_.push_back(thread);
  }
}

void ThreadPool::Stop() {
  boost::unique_lock < boost::mutex > lock(mutex_);
  run_ = false;
  cond_.notify_all();
  for (std::list<ThreadPtr>::iterator it = threads_.begin();
      it != threads_.end(); ++it) {
    (*it)->join();
  }
}

int ThreadPool::ExecuteAsync(const Task& task) {
  boost::unique_lock < boost::mutex > lock(mutex_);
  if (tasks_.size() >= max_task_num_) {
    LOG_WARN("task queue already full.");
    return -1;
  }
  tasks_.push(task);
  cond_.notify_one();
  return 0;
}

void ThreadPool::Run() {
  while (run_) {
    Task task;
    {
      boost::unique_lock < boost::mutex > lock(mutex_);
      while (run_ && tasks_.empty()) {
        cond_.wait(lock);
      }
      task = tasks_.front();
      tasks_.pop();
    }
    if (task) {
      task();
    }
  }
}

}

