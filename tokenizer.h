#pragma once

#include <vector>
#include <iostream>

#include "token.h"

using namespace std;

class Tokenizer {
public:
  vector<Token> tokens;

  Tokenizer(char *);

  friend ostream & operator<<(ostream & os, const Tokenizer & self) {
    os << "T[ ";
    for (const auto token : self.tokens) {
      os << token << " ";
    }
    os << "]";
    return os;
  };

private:
  Token get_token(ifstream *);
  bool is_keyword(string);
  Token wrap_keyword(string);
  Token read_number_token(ifstream *);
  string read_char_list_token(ifstream *);
  Token read_op_token(char);
  bool is_numeric(char);
  bool is_letter(char);
  bool is_op(char);
  bool is_whitespace(char);
};
