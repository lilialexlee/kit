/*
 * thread_pool.h
 *
 */

#ifndef SRC_UTIL_THREAD_POOL_H_
#define SRC_UTIL_THREAD_POOL_H_

#include <queue>
#include <list>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace kit {

class ThreadPool {
 public:
  typedef boost::function<void()> Task;
  typedef boost::shared_ptr<boost::thread> ThreadPtr;

  ThreadPool();
  ~ThreadPool();

  void Init(unsigned int max_tasks, int thread_num);
  void Stop();
  int ExecuteAsync(const Task& task);

 private:
  void Run();

  std::queue<Task> tasks_;
  unsigned int max_task_num_;
  bool run_;
  std::list<ThreadPtr> threads_;
  boost::mutex mutex_;
  boost::condition_variable cond_;

  ThreadPool(const ThreadPool&);
  void operator=(const ThreadPool&);
};

}

#endif /* SRC_UTIL_THREAD_POOL_H_ */
