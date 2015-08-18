/*
 * timer.cc
 *
 */

#include "net/timer.h"
#include <sys/time.h>
#include <climits>
#include <cstddef>

namespace kit {

TimeEvent::TimeEvent(int milliseconds, const TimerCallBack& cb)
    : callback_(cb) {
  gettimeofday(&time_, NULL);
  time_.tv_sec += milliseconds / 1000;
  time_.tv_usec += milliseconds % 1000 * 1000;
}

TimeEvent::~TimeEvent() {
}

void TimeEvent::Process() {
  callback_();
}

struct timeval TimeEvent::GetTime() const {
  return time_;
}

Timer::Timer() {

}

Timer::~Timer() {
}

void Timer::Add(int milliseconds, const TimerCallBack& timer_call_back) {
  TimeEvent event(milliseconds, timer_call_back);
  Add(event);
}

void Timer::Add(const TimeEvent& event) {
  events_[event.GetTime()].push_back(event);
}

int Timer::GetNearestExpiredTime() {
  struct timeval nearest;
  {
    if (events_.empty()) {
      return INT_MAX;
    }
    nearest = events_.begin()->first;
  }
  struct timeval now;
  gettimeofday(&now, NULL);
  int result = (nearest.tv_sec - now.tv_sec) * 1000
      + (nearest.tv_usec - now.tv_usec) % 1000;
  return result > 0 ? result : 0;
}

void Timer::GetExpiredTimer(std::list<TimeEvent>* expired_events) {
  struct timeval now;
  gettimeofday(&now, NULL);
  {
    std::map<timeval, std::list<TimeEvent> >::iterator it = events_.begin();
    for (; it != events_.end() && std::less<timeval>()(it->first, now);) {
      expired_events->insert(expired_events->end(), it->second.begin(),
                             it->second.end());
      events_.erase(it++);
    }
  }
}

}
