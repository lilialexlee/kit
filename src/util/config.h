/*
 * config.h
 *
 *
 * config have tow formats:
 * 1:
 *    keyword argument
 * 2:
 *    keyword argument1
 *    keyword argument2
 *    ...
 *    keyword argumentN
 *
 * key and value are separated by space or tab
 * you use "#" to comment one line
 *
 */

#ifndef SRC_UTIL_CONFIG_H_
#define SRC_UTIL_CONFIG_H_

#include <vector>
#include <string>
#include <map>

namespace kit {

class Config {
 public:
  Config();
  ~Config();

  int Init(const char* filename);

  int GetStr(const std::string& key, std::string* value);
  int GetStrs(const std::string& key, std::vector<std::string>* values);
  int GetInt(const std::string& key, int* value);
  int GetInts(const std::string& key, std::vector<int>* values);

 private:
  typedef std::map<std::string, std::vector<std::string> > ConfigMap;

  ConfigMap contents_;

  Config(const Config&);
  void operator=(const Config&);
};

}

#endif /* SRC_UTIL_CONFIG_H_ */
