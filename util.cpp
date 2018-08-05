#include "util.h"

#include <vector>

ostream& operator<<(ostream& os, const TokenType& self) {
  os << token_e_to_s(self);
  return os;
}

map<string, TokenType> token_translation_map{
    {"T_NAME", TokenType::NAME},
    {"T_NUMBER", TokenType::NUMBER},
    {"T_STRING", TokenType::STRING},
    {"T_ADD", TokenType::OP_ADD},
    {"T_SUB", TokenType::OP_SUB},
    {"T_MUL", TokenType::OP_MUL},
    {"T_DIV", TokenType::OP_DIV},
    {"T_LT", TokenType::OP_LT},
    {"T_GT", TokenType::OP_GT},
    {"T_EOP", TokenType::EOP},
    {"T_COMMA", TokenType::COMMA},
    {"T_SEMICOLON", TokenType::SEMICOLON},
    {"T_BRACE_OPEN", TokenType::BRACE_OPEN},
    {"T_BRACE_CLOSE", TokenType::BRACE_CLOSE},
    {"T_KW_IF", TokenType::KW_IF},
    {"T_KW_WHILE", TokenType::KW_WHILE},
    {"T_KW_FOR", TokenType::KW_FOR},
    {"T_KW_FN", TokenType::KW_FN},
    {"T_PAREN_OPEN", TokenType::PAREN_OPEN},
    {"T_PAREN_CLOSE", TokenType::PAREN_CLOSE},
    {"T_ASSIGN", TokenType::ASSIGN},
    {T_EMPTY, TokenType::EMPTY},
};

map<string, TokenType> keyword_translation_map{
    {"if", TokenType::KW_IF},
    {"while", TokenType::KW_WHILE},
    {"for", TokenType::KW_FOR},
    {"fn", TokenType::KW_FN},
};

bool is_token(string rule_part) {
  return rule_part.at(0) == 'T' && rule_part.at(1) == '_';
}

TokenType token_s_to_e(string s_token) {
  auto it = token_translation_map.find(s_token);
  if (it != token_translation_map.end()) return it->second;
  return TokenType::INVALID;
}

string token_e_to_s(TokenType e_token) {
  for (auto& pair : token_translation_map) {
    if (pair.second == e_token) return pair.first;
  }
  return "";
}

bool token_eq(string s_token, TokenType e_token) {
  return token_s_to_e(s_token) == e_token;
}

template <typename T>
void var_dump(T t) {
  cout << t;
}

template <>
void var_dump<vector<vector<basic_string<char>>>>(
    vector<vector<basic_string<char>>> vs) {
  cout << "{ ";
  for (auto& v : vs) {
    cout << "{ ";
    for (auto& e : v) {
      cout << e << " ";
    }
    cout << "} ";
  }
  cout << "}";
}

template void var_dump<unsigned long>(unsigned long);
template void var_dump<TokenType>(TokenType);
template void var_dump<basic_string<char>>(basic_string<char>);
