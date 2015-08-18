/*
 * pollable_queue.h
 *
 */

#ifndef SRC_NET_POLLABLE_QUEUE_H_
#define SRC_NET_POLLABLE_QUEUE_H_

#include <errno.h>
#include <string.h>
#include <queue>
#include <boost/thread/mutex.hpp>
#include "util/log.h"

namespace kit {

template<class T>
class PollableQueue {
 public:
  PollableQueue();
  ~PollableQueue();

  int Fd();
  int Size();
  bool Empty();
  void Push(const T& item);
  int Pop(T *data);

 private:
  int fds_[2];
  std::queue<T> items_;
  boost::mutex mutex_;

  PollableQueue(const PollableQueue&);
  void operator=(const PollableQueue&);
};

template<class T>
PollableQueue<T>::PollableQueue() {
  if (pipe(fds_) == -1) {
    LOG_ERROR("create pipe fail, error: %d", errno);
  }
}

template<class T>
PollableQueue<T>::~PollableQueue() {
  close(fds_[0]);
  close(fds_[1]);
}

template<class T>
int PollableQueue<T>::Fd() {
  return fds_[0];
}

template<class T>
int PollableQueue<T>::Size() {
  boost::mutex::scoped_lock lock(mutex_);
  return items_.size();
}

template<class T>
bool PollableQueue<T>::Empty() {
  boost::mutex::scoped_lock lock(mutex_);
  return items_.empty();
}

template<class T>
void PollableQueue<T>::Push(const T& item) {
  boost::mutex::scoped_lock lock(mutex_);
  items_.push(item);
  if (::write(fds_[1], "1", 1) == -1) {
    LOG_ERROR("write pipe fail, pipefd: %d, error: %d", fds_[1], errno);
  }
}

template<class T>
int PollableQueue<T>::Pop(T* items) {
  boost::mutex::scoped_lock lock(mutex_);
  if (items_.empty()) {
    return 0;
  }
  *items = items_.front();
  items_.pop();
  char buf[1];
  if (::read(fds_[0], buf, 1) != 1) {
    LOG_ERROR("read pipe fail, pipefd: %d, error: %d", fds_[0], errno);
  }
  return 1;
}

}

#endif /* SRC_NET_POLLABLE_QUEUE_H_ */
