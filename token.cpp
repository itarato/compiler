#include "token.h"
#include "util.h"

Token::Token() : type(TokenType::INVALID), value("") {}
Token::Token(TokenType type, string value) : type(type), value(value) {}
