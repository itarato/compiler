#include "logger.h"
#include <ctime>
#include <iostream>

using namespace std;

StdLogger::StdLogger() {}

void StdLogger::log_for(string type, unsigned int color, string message) {
  auto time_res = time(nullptr);
  char mbstr[100];
  if (strftime(mbstr, sizeof(mbstr), "%H:%M:%S", localtime(&time_res))) {
    cout << "\x1B[1m" << mbstr << " -- ";
  }
  cout << "\x1B[" << color << "m" << type << "\x1B[0m -- \x1B[0m";
  cout << message << endl;
}

void StdLogger::log(string s) { log_for("INFO", 94, s); }

void StdLogger::error(string s) { log_for("ERRO", 91, s); }
