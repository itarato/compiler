#pragma once

#include <iostream>
#include <sstream>

using namespace std;

#define _enum_case(e) case e: os << #e; break;

#define RULE_T_NAME "T_NAME"
#define RULE_T_NUMBER "T_NUMBER"
#define RULE_T_ADD "T_ADD"
#define RULE_T_SUB "T_SUB"
#define RULE_T_MUL "T_MUL"
#define RULE_T_DIV "T_DIV"
#define RULE_T_LT "T_LT"
#define RULE_T_GT "T_GT"
#define RULE_T_EOP "T_EOP"
#define RULE_T_SEMICOLON "T_SEMICOLON"
#define RULE_T_BRACE_OPEN "T_BRACE_OPEN"
#define RULE_T_BRACE_CLOSE "T_BRACE_CLOSE"
#define RULE_T_KW_IF "T_KW_IF"
#define RULE_T_PAREN_OPEN "T_PAREN_OPEN"
#define RULE_T_PAREN_CLOSE "T_PAREN_CLOSE"

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

ostream & operator<<(ostream &, const TokenType &);
