#include "logger.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void Logger::log_all(string &&kind, unsigned int color, string &&s) {
  log(kind, color, forward<string>(s));
}

void Logger::info(string &&s) { log("INFO", 94, forward<string>(s)); }

void Logger::error(string &&s) { log("ERROR", 91, forward<string>(s)); }

void StdLogger::log(string kind, unsigned int color, string &&s) {
  auto time_res = time(nullptr);
  char date_str_buffer[100];
  if (strftime(date_str_buffer, sizeof(date_str_buffer), "%H:%M:%S",
               localtime(&time_res))) {
    cout << "\x1B[1m" << date_str_buffer << " -- ";
  }
  cout << "\x1B[" << color << "m" << kind << "\x1B[0m -- \x1B[0m";
  cout << s << endl;
}
