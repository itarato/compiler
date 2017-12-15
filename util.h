#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <string>

using namespace std;

enum TokenType {
  NAME,

  NUMBER,

  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_LT,
  OP_GT,

  SEMICOLON,

  KW_IF,

  BRACE_OPEN,
  BRACE_CLOSE,

  PAREN_OPEN,
  PAREN_CLOSE,

  EOP,

  INVALID,
};

TokenType token_s_to_e(string);
string token_e_to_s(TokenType);
bool token_eq(string, TokenType);

ostream & operator<<(ostream &, const TokenType &);

bool is_token(string);
