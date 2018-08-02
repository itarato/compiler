#pragma once

#include <string>

using namespace std;

class Logger {
 public:
  virtual void log(string) = 0;
  virtual void error(string) = 0;
};

class StdLogger : Logger {
 public:
  StdLogger();
  void log_for(string, unsigned int, string);
  void log(string);
  void error(string);
};
