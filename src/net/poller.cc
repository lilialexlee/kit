/*
 * poller.cc
 *
 */

#include "net/poller.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "util/log.h"

namespace kit {

FileEvent::FileEvent()
    : fd_(-1),
      registered_mask_(kEventNone),
      fired_mask_(kEventNone),
      callback_() {
}

FileEvent::~FileEvent() {
}

void FileEvent::Process() {
  if (callback_) {
    if (fired_mask_ & kEventError) {
      callback_->ProcessError();
      return;
    }
    if (fired_mask_ & kEventRead) {
      callback_->ProcessRead();
    }
    if (fired_mask_ & kEventWrite) {
      callback_->ProcessWrite();
    }
  }
}

static const int kMaxEventPollOnce = 512;

Poller::Poller() {
  epoll_fd_ = epoll_create(1024); /* 1024 is just a hint for the kernel */
  if (epoll_fd_ == -1) {
    LOG_ERROR("epoll_create fail, error: %d", errno);
  }
  to_be_polled_ = new epoll_event[kMaxEventPollOnce];
}

Poller::~Poller() {
  ::close(epoll_fd_);
  delete[] to_be_polled_;
}

int Poller::GetRegisteredMask(int fd) {
  std::map<int, FileEvent>::iterator it = registered_events_.find(fd);
  if (it == registered_events_.end()) {
    return kEventNone;
  } else {
    return it->second.registered_mask_;
  }
}

int Poller::Set(int fd, int mask, const FileCallBackPtr& cb) {
  if (mask == GetRegisteredMask(fd)) {
    return 0;
  }
  struct epoll_event ee;
  ee.events = 0;
  int registered_mask = mask;
  if (registered_mask & kEventRead) {
    ee.events |= EPOLLIN;
  }
  if (registered_mask & kEventWrite) {
    ee.events |= EPOLLOUT;
  }
  ee.data.u64 = 0;
  ee.data.fd = fd;
  int op =
      registered_events_[fd].registered_mask_ == kEventNone ?
          EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  if (epoll_ctl(epoll_fd_, op, fd, &ee) == -1) {
    LOG_ERROR("epoll_ctl fail, epoll_fd: %d, op: %d, fd: %d, error: %d",
              epoll_fd_, op, fd, errno);
    return -1;
  }
  registered_events_[fd].fd_ = fd;
  registered_events_[fd].registered_mask_ = registered_mask;
  registered_events_[fd].fired_mask_ = 0;
  registered_events_[fd].callback_ = cb;
  return 0;
}

int Poller::Delete(int fd, int del_mask) {
  if (!(del_mask && GetRegisteredMask(fd))) {
    return 0;
  }
  struct epoll_event ee;
  ee.events = 0;
  std::map<int, FileEvent>::iterator it = registered_events_.find(fd);
  if (it == registered_events_.end()) {
    return 0;
  }
  FileEvent& registered_event = it->second;
  int registered_mask = registered_event.registered_mask_ & (~del_mask);
  if (registered_mask & kEventRead) {
    ee.events |= EPOLLIN;
  }
  if (registered_mask & kEventWrite) {
    ee.events |= EPOLLOUT;
  }
  ee.data.u64 = 0;
  ee.data.fd = fd;
  int op = registered_mask == kEventNone ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
  if (epoll_ctl(epoll_fd_, op, fd, &ee) == -1) {
    LOG_ERROR("epoll_ctl fail, epoll_fd: %d, op: %d, fd: %d, error: %d",
              epoll_fd_, op, fd, errno);
    return -1;
  }
  if (registered_mask == kEventNone) {
    registered_events_.erase(fd);
  } else {
    registered_event.registered_mask_ = registered_mask;
  }
  return 0;
}

int Poller::Poll(int timeout_ms, std::vector<FileEvent>* fired_events) {
  int num_events = epoll_wait(epoll_fd_, to_be_polled_, kMaxEventPollOnce,
                              timeout_ms);
  if (num_events == -1) {
    if (errno == EINTR) {
      return 0;
    }
    LOG_ERROR("epoll_wait fail, epoll_fd: %d, error: %d", epoll_fd_, errno);
    return -1;
  }
  fired_events->clear();
  fired_events->resize(num_events);
  for (int j = 0; j < num_events; j++) {
    int fired_mask = 0;
    struct epoll_event *e = to_be_polled_ + j;
    if (e->events & EPOLLIN) {
      fired_mask |= kEventRead;
    }
    if (e->events & EPOLLOUT) {
      fired_mask |= kEventWrite;
    }
    if ((e->events & EPOLLERR) || (e->events & EPOLLHUP)) {
      fired_mask |= kEventError;
    }
    int fd = e->data.fd;
    (*fired_events)[j] = registered_events_[fd];
    (*fired_events)[j].fired_mask_ = fired_mask;
  }
  return num_events;
}

}

