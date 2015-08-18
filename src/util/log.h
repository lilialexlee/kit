/*
 * log.h
 *
 */

#ifndef SRC_UTIL_LOG_H_
#define SRC_UTIL_LOG_H_

#include <cstdarg>
#include <cstdio>
#include <string>
#include <boost/thread/mutex.hpp>

namespace kit {

class Logger {
 public:
  enum LogLevel {
    kTrace = 0,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kFatal,
    kLogLevelNum
  };

  Logger();
  ~Logger();

  static Logger* GetLogger();

  int Init(const char* log_file_name, const char* log_level);

  void LogMessage(LogLevel level, const char *fmt, ...);

 private:
  void Logv(LogLevel level, const char* format, va_list ap);
  void rotate(int year, int month, int day, int hour);

  LogLevel level_;
  FILE* file_;
  std::string log_file_name_;
  int last_hour_;
  boost::mutex mutex_;

  Logger(const Logger&);
  void operator=(const Logger&);
};

int InitLog(const char* log_name, const char* log_level);

#define LOG_TRACE(fmt, args...) \
    kit::Logger::GetLogger()->LogMessage(kit::Logger::kTrace, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define LOG_DEBUG(fmt, args...) \
    kit::Logger::GetLogger()->LogMessage(kit::Logger::kDebug, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define LOG_INFO(fmt, args...) \
    kit::Logger::GetLogger()->LogMessage(kit::Logger::kInfo, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define LOG_WARN(fmt, args...) \
    kit::Logger::GetLogger()->LogMessage(kit::Logger::kWarn, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define LOG_ERROR(fmt, args...) \
    kit::Logger::GetLogger()->LogMessage(kit::Logger::kError, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define LOG_FATAL(fmt, args...) \
    kit::Logger::GetLogger()->LogMessage(kit::Logger::kFatal, "%s(%d): " fmt, __FILE__, __LINE__, ##args)

}

#endif /* SRC_UTIL_LOG_H_ */
