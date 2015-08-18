/*
 * config_test.cc
 *
 */

#include "util/config.h"
#include <iostream>

int main() {
  kit::Config config;
  config.Init("./test/example_conf");

  std::string str_value;
  int ret = config.GetStr("str", &str_value);
  if (ret == 0) {
    std::cout << "get str: " << str_value << "\n";
  } else {
    std::cerr << "get str failed.\n";
  }

  std::vector<std::string> str_values;
  ret = config.GetStrs("strs", &str_values);
  if (ret == 0) {
    std::cout << "get strs:";
    for (size_t i = 0; i < str_values.size(); ++i) {
      std::cout << " " << str_values[i];
    }
    std::cout << "\n";
  } else {
    std::cerr << "get strs failed.\n";
  }

  int int_value;
  ret = config.GetInt("int", &int_value);
  if (ret == 0) {
    std::cout << "get int: " << int_value << "\n";
  } else {
    std::cerr << "get int failed.\n";
  }

  std::vector<int> int_values;
  ret = config.GetInts("ints", &int_values);
  if (ret == 0) {
    std::cout << "get ints:";
    for (size_t i = 0; i < int_values.size(); ++i) {
      std::cout << " " << int_values[i];
    }
    std::cout << "\n";
  } else {
    std::cerr << "get ints failed.\n";
  }

  ret = config.GetStr("not_exist", &str_value);
  if (ret == 0) {
    std::cerr << "get not exist config.\n";
  }

  ret = config.GetStr("comments", &str_value);
  if (ret == 0) {
    std::cerr << "get not exist config.\n";
  }

  return 0;
}

