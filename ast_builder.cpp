#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "ast_builder.h"
#include "ast_node.h"
#include "ast_node_part.h"
#include "grammar.h"
#include "grammar_line.h"
#include "grammar_rule.h"
#include "token.h"
#include "tokenizer.h"
#include "util.h"
#include "globals.h"

using namespace std;

uint8_t indent = 0;

string indent_str() {
  ostringstream oss;
  for (uint8_t i = 0; i < indent; i++) oss << "\x1B[90m| \x1B[0m";
  return oss.str();
}

typedef vector<Token>::iterator vec_iter_t;

AstBuilder::AstBuilder(Grammar *grammar, Tokenizer *tokenizer,
                       bool _verbose_mode)
    : grammar(grammar), tokenizer(tokenizer), verbose_mode(_verbose_mode){};

AstNode *AstBuilder::build() {
  if (verbose_mode) cout << "BUILD" << endl;
  auto it = tokenizer->tokens.begin();
  return try_grammar_line(grammar->lines[START_RULE], &it, START_RULE);
};

AstNode *AstBuilder::try_grammar_line(unique_ptr<GrammarLine> &gr_line,
                                      vec_iter_t *token_it, string rule_name) {
  indent++;

  if (!gr_line) {
    logger.error("Missing grammar line for rule", rule_name);
  }

  if (verbose_mode) {
    cout << indent_str() << "Try line: \x1B[96m" << *gr_line
         << "\x1B[0mon: \x1B[96m" << **token_it << "\x1B[0m" << endl;
  }

  auto orig_token_it = *token_it;

  for (auto rule : gr_line->rules) {
    AstNode *p_ast_node = try_grammar_rule(&rule, token_it, rule_name);
    if (p_ast_node != nullptr) {
      if (verbose_mode) cout << indent_str() << "Y" << endl;
      indent--;
      return p_ast_node;
    }
    if (verbose_mode)
      cout << indent_str() << "Revert \x1B[91m" << **token_it
           << "\x1B[0m -> \x1B[91m" << *orig_token_it << "\x1B[0m" << endl;
    *token_it = orig_token_it;
  }

  if (verbose_mode) cout << indent_str() << "N" << endl;
  indent--;
  return nullptr;
};

AstNode *AstBuilder::try_grammar_rule(GrammarRule *rule, vec_iter_t *token_it,
                                      string rule_name) {
  indent++;

  if (verbose_mode)
    cout << indent_str() << "Try rule: \x1B[93m" << *rule
         << "\x1B[0m on: \x1B[93m" << **token_it << "\x1B[0m" << endl;

  // Make new AstNode
  AstNode *p_ast_node = new AstNode(rule_name);

  for (const auto rule_part : rule->parts) {
    if (is_token(rule_part)) {
      if (is_token_match(rule_part, **token_it)) {
        if (verbose_mode)
          cout << indent_str() << "MATCH \x1B[32m" << **token_it << "\x1B[0m"
               << endl;

        // Push AstNodePart-Token to AstNode
        p_ast_node->parts.push_back(new_ast_node_part_token(**token_it));

        (*token_it)++;
      } else {
        // Abort
        delete p_ast_node;

        if (verbose_mode) cout << indent_str() << "N" << endl;
        indent--;
        return nullptr;
      }
    } else {
      AstNode *p_part_ast_node =
          try_grammar_line(grammar->lines[rule_part], token_it, rule_part);
      if (p_part_ast_node == nullptr) {
        // Abort
        delete p_ast_node;

        if (verbose_mode) cout << indent_str() << "N" << endl;
        indent--;
        return nullptr;
      }

      // Push response
      p_ast_node->parts.push_back(new_ast_node_part_node(p_part_ast_node));
    }
  }

  // register the AstNode
  if (verbose_mode) cout << indent_str() << "Y" << endl;
  indent--;
  return p_ast_node;
}

bool AstBuilder::is_token_match(string rule_part, Token token) {
  return token_eq(rule_part, token.type);
}
