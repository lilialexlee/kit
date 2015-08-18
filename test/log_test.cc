/*
 * log_test.cc
 *
 */

#include "util/log.h"

int main() {
  LOG_INFO("no arg");
  LOG_INFO("with arg: %s", "arg");
  LOG_DEBUG("debug message");
  LOG_WARN("warn message");
  kit::InitLog("test_log", "INFO");
  LOG_INFO("no arg");
  LOG_INFO("with arg: %s", "arg1");
  LOG_DEBUG("debug message");
  LOG_WARN("warn message");
  return 0;
}

