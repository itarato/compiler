#include "ll_ast_builder.h"
#include "ast_node.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <utility>

#include "lookahead_table_generator.h"
#include "token.h"
#include "util.h"

using namespace std;

LLAstBuilder::LLAstBuilder(Grammar *grammar, Tokenizer *t,
                           unsigned int ll_max_level, bool verbose_mode)
    : grammar(grammar), tokenizer(t), ll_max_level(ll_max_level),
      verbose_mode(verbose_mode) {
  LookaheadTableGenerator ltg(grammar, ll_max_level, verbose_mode);

  if (!ltg.generate())
    throw "Lookahead table cannot be generated.";
  rule_lookup = ltg.rule_lookup;
};

AstNode *LLAstBuilder::build() {
  vector<Token> source_tokens;
  copy(tokenizer->tokens.rbegin(), tokenizer->tokens.rend(),
       back_inserter(source_tokens));
  vector<string> rule_tokens({START_RULE});

  AstNode *p_last_ast_node = nullptr;
  vector<pair<AstNode *, uint>> node_stack({});

  while (!rule_tokens.empty() && !source_tokens.empty()) {
    if (verbose_mode) {
      cout << "\x1B[92mR\\\x1B[0m ";
      copy(rule_tokens.rbegin(), rule_tokens.rend(),
           ostream_iterator<string>(cout, " "));
      cout << endl;
      cout << "\x1B[92mS/\x1B[0m ";
      for (auto it = source_tokens.rbegin(); it != source_tokens.rend(); it++)
        cout << token_e_to_s(it->type) << " ";
      cout << endl << endl;
    }

    // If both stack top has the same deterministic token - reduce.
    if (token_eq(rule_tokens.back(), source_tokens.back().type)) {
      if (verbose_mode) {
        cout << "Match - deterministic tokens: " << rule_tokens.back() << endl
             << endl;
      }

      node_stack.back().first->parts.push_back(
          new_ast_node_part_token(source_tokens.back()));

      rule_tokens.pop_back();
      source_tokens.pop_back();
    } else {
      // If rule is deterministic and not matching (prev cond).
      if (is_token(rule_tokens.back())) {
        cout << "Non matching deterministic token.\n";
        exit(EXIT_FAILURE);
      }

      string source_token_string = token_e_to_s(source_tokens.back().type);
      string rule_name = rule_tokens.back();
      int lookup_match = lookup(rule_name, {source_token_string});
      if (lookup_match == -1) {
        // Test for an empty rule and correct the source token if so.
        lookup_match = lookup(rule_tokens.back(), {T_EMPTY});
        if (lookup_match == -1) {
          // From the current first rule the deterministic source token is
          // unreachable.
          cout << "No matching rule.\n";
          exit(EXIT_FAILURE);
        }
        source_token_string = T_EMPTY;
      }

      if (verbose_mode) {
        cout << "Expand rule: " << rule_name << " at #" << lookup_match << endl
             << endl;
      }

      // If rule is obvious - expand.
      node_stack.push_back(
          {new AstNode(rule_tokens.back()), rule_tokens.size() - 1});
      if (verbose_mode) {
        cout << "Added " << rule_tokens.back()
             << " to node stack with marker: " << (rule_tokens.size() - 1)
             << endl;
      }

      rule_tokens.pop_back();
      copy(grammar->lines[rule_name].rules[lookup_match].parts.rbegin(),
           grammar->lines[rule_name].rules[lookup_match].parts.rend(),
           back_inserter(rule_tokens));
    }

    while (node_stack.size() > 0 &&
           rule_tokens.size() == node_stack.back().second) {
      p_last_ast_node = node_stack.back().first;
      node_stack.pop_back();

      if (node_stack.size() > 0) {
        node_stack.back().first->parts.push_back(
            new_ast_node_part_node(p_last_ast_node));
      }
    }
  }

  return p_last_ast_node;
}

int LLAstBuilder::lookup(string rule_name, vector<string> tokens) {
  for (auto it = grammar->lines[rule_name].rules.begin();
       it != grammar->lines[rule_name].rules.end(); it++) {
    unsigned int idx = it - grammar->lines[rule_name].rules.begin();
    vector<vector<string>> &lookup = rule_lookup[{rule_name, idx}];
    for (auto lookup_it = lookup.begin(); lookup_it != lookup.end();
         lookup_it++) {
      unsigned int len = min(tokens.size(), lookup_it->size());

      if (len == 0) {
        cout << "LEN is zero!" << endl;
        exit(EXIT_FAILURE);
      }

      bool is_match = true;
      for (unsigned int i = 0; i < len; i++) {
        if ((*lookup_it)[i] != tokens[i]) {
          is_match = false;
          break;
        }
      }

      if (is_match) {
        return idx;
      }
    }
  }

  return -1;
}
