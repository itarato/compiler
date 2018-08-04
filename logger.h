#pragma once

#include <string>

using namespace std;

class Logger {
 public:
  virtual void log(string, unsigned int, string) = 0;

  void log_all(string, unsigned int, string);

  template <typename T1, typename T2, typename... Args>
  void log_all(string, unsigned int, T1, T2, Args...);

  void info(string);

  template <typename T1, typename T2, typename... Args>
  void info(T1, T2, Args...);

  void error(string);

  template <typename T1, typename T2, typename... Args>
  void error(T1, T2, Args...);
};

class StdLogger : public Logger {
 public:
  void log(string, unsigned int, string);
};