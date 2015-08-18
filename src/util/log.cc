/*
 * log.cc
 *
 */

#include "util/log.h"
#include <sys/time.h>
#include <cstddef>
#include <cstring>

namespace kit {

static const char* const kLogLevel[] = { "TRACE", "DEBUG", "INFO", "WARN",
    "ERROR", "FATAL" };

static const int kMaxLogFileNameLength = 128;

static const int kMaxLogLength = 4096;

Logger::Logger()
    : level_(kInfo),
      file_(stdout),
      log_file_name_(),
      last_hour_(-1),
      mutex_() {
}

Logger::~Logger() {
  if (file_ != stdout && file_ != stdin && file_ != stderr) {
    fclose(file_);
  }
}

int Logger::Init(const char* log_file_name, const char* log_level) {
  if (log_file_name == NULL
      || strlen(log_file_name) > kMaxLogFileNameLength|| log_level == NULL) {
    return -1;
  }
  int i = 0;
  for (; i < static_cast<int>(kLogLevelNum); ++i) {
    if (strcasecmp(log_level, kLogLevel[i]) == 0) {
      level_ = static_cast<LogLevel>(i);
      break;
    }
  }
  if (i == kLogLevelNum) {
    return -1;
  }
  FILE *f = fopen(log_file_name, "a");
  if (f == NULL) {
    return -1;
  }
  file_ = f;
  log_file_name_ = log_file_name;
  struct timeval now_tv;
  gettimeofday(&now_tv, NULL);
  const time_t seconds = now_tv.tv_sec;
  struct tm t;
  localtime_r(&seconds, &t);
  last_hour_ = t.tm_hour;
  return 0;
}

void Logger::LogMessage(LogLevel level, const char *fmt, ...) {
  if (level >= level_) {
    va_list ap;
    va_start(ap, fmt);
    Logv(level, fmt, ap);
    va_end(ap);
  }
}

void Logger::Logv(LogLevel level, const char* format, va_list ap) {
  pthread_t tid = pthread_self();
  struct timeval now_tv;
  gettimeofday(&now_tv, NULL);
  const time_t seconds = now_tv.tv_sec;
  struct tm t;
  localtime_r(&seconds, &t);
  char buf[kMaxLogLength];
  char* p = buf;
  char * limit = buf + kMaxLogLength;
  int off = snprintf(p, limit - p,
                     "%04d/%02d/%02d-%02d:%02d:%02d.%06d %llx %s ",
                     t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour,
                     t.tm_min, t.tm_sec, static_cast<int>(now_tv.tv_usec),
                     static_cast<long long unsigned int>(tid),
                     kLogLevel[static_cast<int>(level)]);
  if (off < kMaxLogLength) {
    p += off;
    off += vsnprintf(p, limit - p, format, ap);
    p = buf + off;
  }
  if (off >= kMaxLogLength) {
    p = limit - 1;
  }
  // Add newline if necessary
  if (p == buf || *(p - 1) != '\n') {
    *p++ = '\n';
  }
  boost::mutex::scoped_lock lock(mutex_);
  if (!log_file_name_.empty() && t.tm_hour != last_hour_) {
    rotate(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, last_hour_);
  }
  fwrite(buf, 1, p - buf, file_);
  fflush(file_);
}

void Logger::rotate(int year, int month, int day, int hour) {
  char last_log[kMaxLogFileNameLength];
  snprintf(last_log, kMaxLogFileNameLength, "%s.%04d%02d%02d%02d",
           log_file_name_.c_str(), year, month, day, hour);
  fclose(file_);
  rename(log_file_name_.c_str(), last_log);
  file_ = fopen(log_file_name_.c_str(), "a");
  last_hour_ = hour;
}

static pthread_once_t once = PTHREAD_ONCE_INIT;
static Logger* logger;
static void CreateLogger() {
  logger = new Logger;
}

Logger* Logger::GetLogger() {
  pthread_once(&once, CreateLogger);
  return logger;
}

int InitLog(const char* log_name, const char* log_level) {
  return Logger::GetLogger()->Init(log_name, log_level);
}

}
