#include "util.h"

ostream & operator<<(ostream & os, const TokenType & self) {
  switch (self) {
    _enum_case(NAME);
    _enum_case(NUMBER);
    _enum_case(OP_ADD);
    _enum_case(OP_SUB);
    _enum_case(OP_MUL);
    _enum_case(OP_DIV);
    _enum_case(OP_LT);
    _enum_case(OP_GT);
    _enum_case(SEMICOLON);
    _enum_case(EOP);
    _enum_case(KW_IF);
    _enum_case(BRACE_OPEN);
    _enum_case(BRACE_CLOSE);
    _enum_case(PAREN_OPEN);
    _enum_case(PAREN_CLOSE);
    _enum_case(INVALID);
  }
  return os;
}
