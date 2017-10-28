#include <vector>
#include <iostream>
#include <sstream>

#include "ast_builder.h"
#include "token.h"
#include "tokenizer.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "grammar_line.h"

using namespace std;

uint8_t indent = 0;

string indent_str() {
  ostringstream oss;
  for (uint8_t i = 0; i < indent; i++) oss << "\e[90m| \e[0m";
  return oss.str();
}

typedef vector<Token>::iterator vec_iter_t;

AstBuilder::AstBuilder(Grammar *grammar, Tokenizer *tokenizer) : grammar(grammar), tokenizer(tokenizer) {};

bool AstBuilder::build() {
  cout << "BUILD" << endl;
  vec_iter_t it = tokenizer->tokens.begin();
  return try_grammar_line(&(grammar->lines["PROG"]), &it);
};

bool AstBuilder::try_grammar_line(GrammarLine *gr_line, vec_iter_t *token_it) {
  indent++;

  cout << indent_str() << "Try line: \e[96m" << *gr_line << "\e[0mon: \e[96m" << **token_it << "\e[0m" << endl;
  auto orig_token_it = *token_it;

  for (auto rule : gr_line->rules) {
    if (try_grammar_rule(&rule, token_it)) {
      cout << indent_str() << "Y" << endl;
      indent--;
      return true;
    }
    cout << indent_str() << "Revert  \e[91m" << **token_it << "\e[0m -> \e[91m" << *orig_token_it << "\e[0m" << endl;
    *token_it = orig_token_it;
  }

  cout << indent_str() << "N" << endl;
  indent--;
  return false;
};

bool AstBuilder::try_grammar_rule(GrammarRule *rule, vec_iter_t *token_it) {
  indent++;

  cout << indent_str() << "Try rule: \e[93m" << *rule << "\e[0m on: \e[93m" << **token_it << "\e[0m" << endl;

  for (const auto rule_part : rule->parts) {
    if (is_token(rule_part)) {
      if (is_token_match(rule_part, **token_it)) {
        cout << indent_str() << "MATCH \e[32m" << **token_it << "\e[0m" << endl;
        (*token_it)++;
      } else {
        cout << indent_str() << "N" << endl;
        indent--;
        return false;
      }
    } else {
      if (!try_grammar_line(&grammar->lines[rule_part], token_it)) {
        cout << indent_str() << "N" << endl;
        indent--;
        return false;
      }
    }
  }

  cout << indent_str() << "Y" << endl;
  indent--;
  return true;
}

bool AstBuilder::is_token(string rule_part) {
  return rule_part.at(0) == 'T' && rule_part.at(1) == '_';
}

bool AstBuilder::is_token_match(string rule_part, Token token) {
  return
    (rule_part == RULE_T_NAME && token.type == TokenType::NAME) ||
    (rule_part == RULE_T_NUMBER && token.type == TokenType::NUMBER) ||
    (rule_part == RULE_T_ADD && token.type == TokenType::OP_ADD) ||
    (rule_part == RULE_T_SUB && token.type == TokenType::OP_SUB) ||
    (rule_part == RULE_T_MUL && token.type == TokenType::OP_MUL) ||
    (rule_part == RULE_T_DIV && token.type == TokenType::OP_DIV) ||
    (rule_part == RULE_T_LT && token.type == TokenType::OP_LT) ||
    (rule_part == RULE_T_GT && token.type == TokenType::OP_GT) ||
    (rule_part == RULE_T_SEMICOLON && token.type == TokenType::SEMICOLON) ||
    (rule_part == RULE_T_EOP && token.type == TokenType::EOP) ||
    (rule_part == RULE_T_BRACE_OPEN && token.type == TokenType::BRACE_OPEN) ||
    (rule_part == RULE_T_BRACE_CLOSE && token.type == TokenType::BRACE_CLOSE) ||
    (rule_part == RULE_T_KW_IF && token.type == TokenType::KW_IF);
}
