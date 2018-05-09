#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define T_EMPTY "T_EMPTY"
#define START_RULE "PROG"

enum TokenType {
    NAME,

    NUMBER,
    STRING,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LT,
    OP_GT,

    ASSIGN,

    COMMA,
    SEMICOLON,

    KW_IF,
    KW_WHILE,
    KW_FOR,
    KW_FN,

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

extern map<string, TokenType> token_translation_map;
extern map<string, TokenType> keyword_translation_map;

template<typename T> extern void var_dump(T);
template<> void var_dump<vector<vector<basic_string<char>>>>(vector<vector<basic_string<char>>>);
