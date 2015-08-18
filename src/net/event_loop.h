/*
 * event_loop.h
 *
 */

#ifndef SRC_NET_EVENT_LOOP_H_
#define SRC_NET_EVENT_LOOP_H_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "net/poller.h"
#include "net/timer.h"
#include "net/pollable_queue.h"

namespace kit {

class EventLoop {
 public:
  typedef boost::function<void()> CallBack;
  typedef PollableQueue<CallBack> WakeUpQueue;
  typedef boost::shared_ptr<WakeUpQueue> WakeUpQueuePtr;

  EventLoop();
  ~EventLoop();

  void Loop();
  void Stop();

  int GetRegisteredFileMask(int fd);
  int SetFileEvent(int fd, int mask, const FileCallBackPtr& cb);
  int DeleteFileEvent(int fd, int del_mask);

  void AddTimeEvent(int milliseconds, const TimerCallBack& timer_call_back);
  void AddTimeEvent(const TimeEvent& event);

  void RunInLoopThread(const CallBack& cb);

 private:
  void WakeUp(const CallBack& cb);

  Poller poller_;
  Timer timer_;
  WakeUpQueuePtr wake_up_queue_;
  pthread_t loop_thread_id_;
  bool run_;

  EventLoop(const EventLoop&);
  void operator=(const EventLoop&);
};

}

#endif /* SRC_NET_EVENT_LOOP_H_ */
