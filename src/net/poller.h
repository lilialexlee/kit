/*
 * poller.h
 *
 */

#ifndef SRC_NET_POLLER_H_
#define SRC_NET_POLLER_H_

#include <sys/epoll.h>
#include <cstddef>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace kit {

class FileCallBack {
 public:
  FileCallBack() {
  }

  virtual ~FileCallBack() {
  }

  virtual void ProcessRead() = 0;
  virtual void ProcessWrite() = 0;
  virtual void ProcessError() = 0;
};

typedef boost::shared_ptr<FileCallBack> FileCallBackPtr;

static const int kEventNone = 0;
static const int kEventRead = (1 << 0);
static const int kEventWrite = (1 << 1);
static const int kEventError = (1 << 2);

class FileEvent {
 public:
  FileEvent();
  ~FileEvent();

  void Process();

 private:
  friend class Poller;
  int fd_;
  int registered_mask_;
  int fired_mask_;
  FileCallBackPtr callback_;
};

class Poller {
 public:
  Poller();
  ~Poller();

  int GetRegisteredMask(int fd);
  int Set(int fd, int mask, const FileCallBackPtr& cb);
  int Delete(int fd, int del_mask);
  int Poll(int timeout_ms, std::vector<FileEvent>* fired_events);

 private:
  int epoll_fd_;
  std::map<int, FileEvent> registered_events_;
  struct epoll_event* to_be_polled_;

  Poller(const Poller&);
  void operator=(const Poller&);
};

}

#endif /* SRC_NET_POLLER_H_ */
