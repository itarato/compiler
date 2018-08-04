#include "logger.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void Logger::log_all(string kind, unsigned int color, string s) {
  log(kind, color, s);
}

template <typename T1, typename T2, typename... Args>
void Logger::log_all(string kind, unsigned int color, T1 lhs, T2 rhs,
                     Args... args) {
  ostringstream buf{};
  buf << lhs << " " << rhs;
  log_all(kind, color, buf.str(), args...);
}

void Logger::info(string s) { log("INFO", 94, s); }

template <typename T1, typename T2, typename... Args>
void Logger::info(T1 lhs, T2 rhs, Args... args) {
  log_all("INFO", 94, lhs, rhs, args...);
}

void Logger::error(string s) { log("ERROR", 91, s); }

template <typename T1, typename T2, typename... Args>
void Logger::error(T1 lhs, T2 rhs, Args... args) {
  log_all("ERROR", 91, lhs, rhs, args...);
}

void StdLogger::log(string kind, unsigned int color, string s) {
  auto time_res = time(nullptr);
  char mbstr[100];
  if (strftime(mbstr, sizeof(mbstr), "%H:%M:%S", localtime(&time_res))) {
    cout << "\x1B[1m" << mbstr << " -- ";
  }
  cout << "\x1B[" << color << "m" << kind << "\x1B[0m -- \x1B[0m";
  cout << s << endl;
}
