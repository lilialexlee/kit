/*
 * event_loop.cc
 *
 */

#include <boost/bind.hpp>
#include "net/event_loop.h"
#include "util/log.h"

namespace kit {

namespace {

class EventLoopWakeUpCallBack : public FileCallBack {
 public:
  EventLoopWakeUpCallBack(const EventLoop::WakeUpQueuePtr& wake_up_queue)
      : wake_up_queue_(wake_up_queue) {
  }

  ~EventLoopWakeUpCallBack() {
  }

  void ProcessRead() {
    EventLoop::CallBack cb;
    while (wake_up_queue_->Pop(&cb) > 0) {
      cb();
    }
  }

  void ProcessWrite() {
  }

  void ProcessError() {
  }

 private:
  EventLoop::WakeUpQueuePtr wake_up_queue_;
};

}

EventLoop::EventLoop()
    : poller_(),
      timer_(),
      wake_up_queue_(new WakeUpQueue),
      loop_thread_id_(0),
      run_(true) {
}

EventLoop::~EventLoop() {
}

static const int kMaxLoopInterval = 100;

void EventLoop::Loop() {
  loop_thread_id_ = pthread_self();
  FileCallBackPtr cb(new EventLoopWakeUpCallBack(wake_up_queue_));
  poller_.Set(wake_up_queue_->Fd(), kEventRead, cb);
  LOG_INFO("start loop....");
  while (run_) {
    int timeout_ms = timer_.GetNearestExpiredTime();
    timeout_ms = std::min(timeout_ms, kMaxLoopInterval);
    std::vector<FileEvent> fired_file_events;
    int ret = poller_.Poll(timeout_ms, &fired_file_events);
    LOG_TRACE("poll wait return %d.", ret);
    if (ret > 0) {
      for (std::vector<FileEvent>::iterator it = fired_file_events.begin();
          it != fired_file_events.end(); ++it) {
        it->Process();
      }
    }
    std::list<TimeEvent> expired_time_events;
    timer_.GetExpiredTimer(&expired_time_events);
    for (std::list<TimeEvent>::iterator it = expired_time_events.begin();
        it != expired_time_events.end(); ++it) {
      it->Process();
    }
  }
}

void EventLoop::Stop() {
  run_ = false;
}

int EventLoop::GetRegisteredFileMask(int fd) {
  return poller_.GetRegisteredMask(fd);
}

int EventLoop::SetFileEvent(int fd, int mask, const FileCallBackPtr& cb) {
  return poller_.Set(fd, mask, cb);
}

int EventLoop::DeleteFileEvent(int fd, int del_mask) {
  return poller_.Delete(fd, del_mask);
}

void EventLoop::AddTimeEvent(int milliseconds,
                             const TimerCallBack& timer_call_back) {
  TimeEvent event(milliseconds, timer_call_back);
  timer_.Add(event);
}

void EventLoop::AddTimeEvent(const TimeEvent& event) {
  timer_.Add(event);
}

void EventLoop::RunInLoopThread(const CallBack& cb) {
  if (loop_thread_id_ == pthread_self()) {
    cb();
  } else {
    WakeUp(cb);
  }
}

void EventLoop::WakeUp(const CallBack& cb) {
  wake_up_queue_->Push(cb);
}

}

