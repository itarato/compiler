#include <vector>
#include <iostream>
#include <sstream>
#include <utility>

#include "ast_builder.h"
#include "token.h"
#include "tokenizer.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "grammar_line.h"
#include "ast_node_part.h"
#include "ast_node.h"

using namespace std;

uint8_t indent = 0;

string indent_str() {
  ostringstream oss;
  for (uint8_t i = 0; i < indent; i++) oss << "\x1B[90m| \x1B[0m";
  return oss.str();
}

typedef vector<Token>::iterator vec_iter_t;

AstBuilder::AstBuilder(Grammar *grammar, Tokenizer *tokenizer) : grammar(grammar), tokenizer(tokenizer) {};

AstNode * AstBuilder::build() {
  cout << "BUILD" << endl;
  vec_iter_t it = tokenizer->tokens.begin();
  return try_grammar_line(&(grammar->lines["PROG"]), &it);
};

AstNode * AstBuilder::try_grammar_line(GrammarLine *gr_line, vec_iter_t *token_it) {
  indent++;

  cout << indent_str() << "Try line: \x1B[96m" << *gr_line << "\x1B[0mon: \x1B[96m" << **token_it << "\x1B[0m" << endl;
  auto orig_token_it = *token_it;

  for (auto rule : gr_line->rules) {
    AstNode *p_ast_node = try_grammar_rule(&rule, token_it);
    if (p_ast_node != nullptr) {
      cout << indent_str() << "Y" << endl;
      indent--;
      return p_ast_node;
    }
    cout << indent_str() << "Revert  \x1B[91m" << **token_it << "\x1B[0m -> \x1B[91m" << *orig_token_it << "\x1B[0m" << endl;
    *token_it = orig_token_it;
  }

  cout << indent_str() << "N" << endl;
  indent--;
  return nullptr;
};

AstNode * AstBuilder::try_grammar_rule(GrammarRule *rule, vec_iter_t *token_it) {
  indent++;

  cout << indent_str() << "Try rule: \x1B[93m" << *rule << "\x1B[0m on: \x1B[93m" << **token_it << "\x1B[0m" << endl;

  // Make new AstNode
  AstNode *p_ast_node = new AstNode("rule-later");

  for (const auto rule_part : rule->parts) {
    if (is_token(rule_part)) {
      if (is_token_match(rule_part, **token_it)) {
        cout << indent_str() << "MATCH \x1B[32m" << **token_it << "\x1B[0m" << endl;

        // Push AstNodePart-Token to AstNode
        p_ast_node->parts.push_back(new_ast_node_part_token(**token_it));

        (*token_it)++;
      } else {
        // Abort

        delete p_ast_node;

        cout << indent_str() << "N" << endl;
        indent--;
        return nullptr;
      }
    } else {
      AstNode * p_part_ast_node = try_grammar_line(&grammar->lines[rule_part], token_it);
      if (p_part_ast_node == nullptr) {
        // Abort

        delete p_ast_node;

        cout << indent_str() << "N" << endl;
        indent--;
        return nullptr;
      }

      // Push response
      p_ast_node->parts.push_back(new_ast_node_part_node(p_part_ast_node));
    }
  }

  // register the AstNode

  cout << indent_str() << "Y" << endl;
  indent--;
  return p_ast_node;
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
