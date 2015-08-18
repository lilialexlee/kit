/*
 * config.cc
 *
 */

#include "util/config.h"
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cstring>
#include "util/log.h"
#include "util/str_util.h"

namespace kit {

static const int kConfigMaxLine = 1024;

Config::Config() {
}

Config::~Config() {
}

int Config::Init(const char* filename) {
  if (filename == NULL || filename[0] == '\0') {
    LOG_ERROR("config file invalid.");
    return -1;
  }
  FILE *f;
  if ((f = fopen(filename, "r")) == NULL) {
    LOG_ERROR("open config file %s error", filename);
    return -1;
  }
  char buf[kConfigMaxLine];
  while (fgets(buf, sizeof(buf), f)) {
    buf[strlen(buf) - 1] = '\0';  // remove '\n'
    trim(buf);
    if (buf[0] == '#' || buf[0] == '\0') {
      continue;
    }
    std::vector<std::string> args = str_split(buf, " \t");
    if (args.size() != 2) {
      return -1;
    }
    contents_[args[0]].push_back(args[1]);
  }
  if (ferror(f)) {
    LOG_ERROR("read config file %s error", filename);
    return -1;
  }
  fclose(f);
  return 0;
}

int Config::GetStr(const std::string& key, std::string* value) {
  std::vector<std::string> values;
  int ret = GetStrs(key, &values);
  if (ret < 0) {
    return ret;
  }
  if (values.size() != 1) {
    return -1;
  }
  *value = values.front();
  return 0;
}

int Config::GetStrs(const std::string& key, std::vector<std::string>* values) {
  ConfigMap::iterator it = contents_.find(key);
  if (it == contents_.end()) {
    return -1;
  }
  values->assign(it->second.begin(), it->second.end());
  return 0;
}

int Config::GetInt(const std::string& key, int* value) {
  std::vector<int> values;
  int ret = GetInts(key, &values);
  if (ret < 0) {
    return ret;
  }
  if (values.size() != 1) {
    return -1;
  }
  *value = values.front();
  return 0;
}

int Config::GetInts(const std::string& key, std::vector<int>* values) {
  std::vector<std::string> str_values;
  int ret = GetStrs(key, &str_values);
  if (ret < 0) {
    return ret;
  }
  values->clear();
  for (std::vector<std::string>::iterator it = str_values.begin();
      it != str_values.end(); ++it) {
    long value;
    int ret = str_to_long(*it, 10, &value);
    if (ret < 0 || value > INT_MAX || value < INT_MIN) {
      values->clear();
      return -1;
    } else {
      values->push_back(value);
    }
  }
  return 0;
}

}

