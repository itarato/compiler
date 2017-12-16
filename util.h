#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;

#define T_EMPTY "T_EMPTY"
#define START_RULE "PROG"

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

    INVALID,  // Represents invalid token.
    EMPTY,    // Represents empty space (for instant reducable rules).
};

TokenType token_s_to_e(string);
string token_e_to_s(TokenType);
bool token_eq(string, TokenType);

ostream &operator<<(ostream &, const TokenType &);

bool is_token(string);
