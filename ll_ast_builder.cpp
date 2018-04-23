#include "ll_ast_builder.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <utility>
#include "ast_node.h"

#include "token.h"
#include "util.h"

using namespace std;

void print_flat_grammar_rules(vector<FlatGrammarRule> fgr) {
  cout << "Grammar / decision list:\n";
  unsigned int idx = 0;
  for (auto &rule : fgr) {
    cout << "#" << idx++ << " " << rule.rule_name << " :> " << rule.rule
         << endl;
  }
}

LLAstBuilder::LLAstBuilder(Grammar *g, Tokenizer *t, unsigned int ll_level)
    : grammar(g), tokenizer(t), ll_level(ll_level) {
  build_flat_grammar_version();
};

AstNode *LLAstBuilder::build() {
  print_flat_grammar_rules(flat_grammar);

  vector<Token> source_tokens;
  copy(tokenizer->tokens.rbegin(), tokenizer->tokens.rend(),
       back_inserter(source_tokens));
  vector<string> rule_tokens({START_RULE});

  AstNode *p_last_ast_node = nullptr;
  vector<pair<AstNode *, uint>> node_stack({});

  while (!rule_tokens.empty() && !source_tokens.empty()) {
    cout << "\x1B[92mR\\\x1B[0m ";
    copy(rule_tokens.rbegin(), rule_tokens.rend(),
         ostream_iterator<string>(cout, " "));
    cout << endl;
    cout << "\x1B[92mS/\x1B[0m ";
    for (auto it = source_tokens.rbegin(); it != source_tokens.rend(); it++)
      cout << token_e_to_s(it->type) << " ";
    cout << endl << endl;

    // If both stack top has the same deterministic token - reduce.
    if (token_eq(rule_tokens.back(), source_tokens.back().type)) {
      cout << "Match - deterministic tokens: " << rule_tokens.back() << endl
           << endl;

      node_stack.back().first->parts.push_back(
          new_ast_node_part_token(source_tokens.back()));

      rule_tokens.pop_back();
      source_tokens.pop_back();

      while (node_stack.size() > 0 &&
             rule_tokens.size() == node_stack.back().second) {
        p_last_ast_node = node_stack.back().first;
        node_stack.pop_back();

        if (node_stack.size() > 0) {
          node_stack.back().first->parts.push_back(
              new_ast_node_part_node(p_last_ast_node));
        }
      }
    } else {
      // If rule is deterministic and not matching (prev cond).
      if (is_token(rule_tokens.back())) {
        cout << "Non matching deterministic token.\n";
        exit(EXIT_FAILURE);
      }

      string source_token_string = token_e_to_s(source_tokens.back().type);
      int lookup_match = lookup(rule_tokens.back(), {source_token_string});
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

      cout << "Expand rule: #" << lookup_match << endl << endl;

      // If rule is obvious - expand.
      node_stack.push_back(
          {new AstNode(rule_tokens.back()), rule_tokens.size() - 1});
      cout << "Added " << rule_tokens.back()
           << " to node stack with marker: " << (rule_tokens.size() - 1)
           << endl;

      rule_tokens.pop_back();
      copy(flat_grammar[lookup_match].rule.parts.rbegin(),
           flat_grammar[lookup_match].rule.parts.rend(),
           back_inserter(rule_tokens));
    }
  }

  return p_last_ast_node;
}

void LLAstBuilder::build_flat_grammar_version() {
  unsigned int idx = 0;
  for (auto &line_pair : grammar->lines) {
    string rule_name(line_pair.first);
    for (auto &rule : line_pair.second.rules) {
      flat_grammar.push_back({rule_name, rule});
      vector<vector<string>> reached_tokens_list = find_starting_tokens(rule);
      for (auto &reached_tokens : reached_tokens_list) {
        // Validate grammar - LL1 is not that strong - easy to have
        // multiple nonterminals for terminals.

        int lookup_match = lookup(rule_name, reached_tokens);
        if (lookup_match >= 0) {
          cout << "Invalid grammar - terminal token can be reached "
                  "from multiple non terminal.\n";
          cout << "Collision: " << rule_name << " - ( ";
          copy(reached_tokens.begin(), reached_tokens.end(),
               ostream_iterator<string>(cout, " "));
          cout << ") : " << lookup_match << " vs " << idx << " in\n";
          print_flat_grammar_rules(flat_grammar);
          exit(EXIT_FAILURE);
        }

        rule_lookup[rule_name][(unsigned int)idx].push_back(reached_tokens);
      }

      idx++;
    }
  }

  print_rule_lookup();
}

vector<vector<string>> LLAstBuilder::find_starting_tokens(GrammarRule rule) {
  if (rule.parts.size() == 0) return {{T_EMPTY}};

  vector<vector<string>> out;
  if (is_token(rule.parts[0])) {
    out.push_back({rule.parts[0]});
  } else {
    for (auto &_rule : grammar->lines[rule.parts[0]].rules) {
      auto sub_out = find_starting_tokens(_rule);
      copy(sub_out.begin(), sub_out.end(), back_inserter(out));
    }
  }

  return out;
}

int LLAstBuilder::lookup(string rule_name, vector<string> tokens) {
  if (rule_lookup.find(rule_name) == rule_lookup.end()) return -1;

  for (auto tokens_pair : rule_lookup[rule_name]) {
    for (auto lookup_tokens : tokens_pair.second) {
      if (equal(lookup_tokens.begin(), lookup_tokens.end(), tokens.begin())) {
        return tokens_pair.first;
      }
    }
  }

  return -1;
}

void LLAstBuilder::print_rule_lookup() {
  cout << "Rule lookup matrix:\n";

  for (auto &rule_lookup_pair : rule_lookup) {
    for (auto &tokens_pair : rule_lookup_pair.second) {
      for (auto &tokens : tokens_pair.second) {
        cout << "[" << rule_lookup_pair.first << ", ( ";
        copy(tokens.begin(), tokens.end(), ostream_iterator<string>(cout, " "));
        cout << ")] -> " << tokens_pair.first << endl;
      }
    }
  }
}
