#pragma once

#include <iostream>
#include <string>

#include "util.h"

using namespace std;

class Token {
public:
  TokenType type;
  string value;

  Token();
  Token(TokenType, string);

  friend ostream & operator<<(ostream & os, const Token & self) {
    os << self.type << "(" << self.value << ")";
    return os;
  };
};
