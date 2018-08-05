#include "logger.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void Logger::log_all(string kind, unsigned int color, string s) {
  log(kind, color, s);
}

void Logger::info(string s) { log("INFO", 94, s); }

void Logger::error(string s) { log("ERROR", 91, s); }

void StdLogger::log(string kind, unsigned int color, string s) {
  auto time_res = time(nullptr);
  char mbstr[100];
  if (strftime(mbstr, sizeof(mbstr), "%H:%M:%S", localtime(&time_res))) {
    cout << "\x1B[1m" << mbstr << " -- ";
  }
  cout << "\x1B[" << color << "m" << kind << "\x1B[0m -- \x1B[0m";
  cout << s << endl;
}
