/*
 * timer.h
 *
 */

#ifndef SRC_NET_TIMER_H_
#define SRC_NET_TIMER_H_

#include <sys/time.h>
#include <functional>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace std {

template<>
struct less<timeval> {
  bool operator()(const timeval& x, const timeval& y) const {
    return (x.tv_sec < y.tv_sec)
        || (x.tv_sec == y.tv_sec && x.tv_usec < y.tv_usec);
  }
};

}

namespace kit {

typedef boost::function<void()> TimerCallBack;

class TimeEvent {
 public:
  TimeEvent(int milliseconds, const TimerCallBack& cb);
  ~TimeEvent();

  void Process();
  struct timeval GetTime() const;

 private:
  struct timeval time_;
  TimerCallBack callback_;
};

class Timer {
 public:
  ;
  Timer();
  ~Timer();

  void Add(int milliseconds, const TimerCallBack& timer_call_back);
  void Add(const TimeEvent& event);
  int GetNearestExpiredTime();
  void GetExpiredTimer(std::list<TimeEvent>* expired_events);

 private:
  std::map<timeval, std::list<TimeEvent> > events_;

  Timer(const Timer&);
  void operator=(const Timer&);

};

}

#endif /* SRC_NET_TIMER_H_ */
