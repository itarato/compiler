#pragma once

#include <sstream>
#include <string>

using namespace std;

class Logger {
 public:
  virtual void log(string, unsigned int, string) = 0;

  void log_all(string, unsigned int, string);

  template <typename T1, typename T2, typename... Args>
  inline void log_all(string kind, unsigned int color, T1 lhs, T2 rhs,
                      Args... args) {
    ostringstream buf{};
    buf << lhs << " " << rhs;
    log_all(kind, color, buf.str(), args...);
  }

  void info(string);

  template <typename T1, typename T2, typename... Args>
  inline void info(T1 lhs, T2 rhs, Args... args) {
    log_all("INFO", 94, lhs, rhs, args...);
  }

  void error(string);

  template <typename T1, typename T2, typename... Args>
  inline void error(T1 lhs, T2 rhs, Args... args) {
    log_all("ERROR", 91, lhs, rhs, args...);
  }
};

class StdLogger : public Logger {
 public:
  void log(string, unsigned int, string);
};
