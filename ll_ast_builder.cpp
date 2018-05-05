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

LLAstBuilder::LLAstBuilder(Grammar *g, Tokenizer *t, unsigned int ll_max_level, bool _verbose_mode)
    : grammar(g), tokenizer(t), ll_max_level(ll_max_level), verbose_mode(_verbose_mode) {

  bool found_valid_lookup_table = false;
  for (unsigned int ll_level = 1; ll_level <= ll_max_level; ll_level++) {
    cout << "Looking for LL(" << ll_level << ") lookup table.\n";

    build_lookup_table(ll_level);
    if (is_lookup_table_valid()) {
      found_valid_lookup_table = true;
      break;
    }
  }

  if (!found_valid_lookup_table) {
    cout << "Error. No possible LL(" << ll_max_level << ") parser.\n";
    exit(EXIT_FAILURE);
  }
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
      string rule_name = rule_tokens.back();
      int lookup_match = lookup(rule_name, {source_token_string});
      if (lookup_match == -1) {
  //       // Test for an empty rule and correct the source token if so.
  //       lookup_match = lookup(rule_tokens.back(), {T_EMPTY});
  //       if (lookup_match == -1) {
  //         // From the current first rule the deterministic source token is
  //         // unreachable.
          cout << "No matching rule.\n";
          exit(EXIT_FAILURE);
  //       }
  //       source_token_string = T_EMPTY;
      }

      if (verbose_mode) {
        cout << "Expand rule: " << rule_name << " at #" << lookup_match << endl << endl;
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
  }

  return p_last_ast_node;
}

void LLAstBuilder::build_lookup_table(unsigned int ll_level) {
  for (auto & grammar_line_pair : grammar->lines) {
    for (auto rule_it = grammar_line_pair.second.rules.begin(); rule_it != grammar_line_pair.second.rules.end(); rule_it++) {
      unsigned int idx = rule_it - grammar_line_pair.second.rules.begin();

      rule_lookup[{grammar_line_pair.first, idx}] = find_starting_tokens(grammar_line_pair.first, idx, ll_level);
    }
  }

  if (verbose_mode) print_rule_lookup();
}

// @TODO Add lookup row uniqueness check!
vector<vector<string>> LLAstBuilder::find_starting_tokens(string rule_name, unsigned int idx, int limit) {
  vector<vector<string>> parts({{}});

  for (auto & _part : grammar->lines[rule_name].rules[idx].parts) {
    if (!any_of(parts.begin(), parts.end(), [&limit](auto p){ return p.size() < limit; })) continue;

    if (is_token(_part)) {
      transform(parts.begin(), parts.end(), parts.begin(), [&limit, &_part](auto p){
        if (p.size() >= limit) {
          return p;
        } else {
          p.push_back(_part);
          return p;
        }
      });
    } else {
      vector<vector<string>> new_parts({});

      for (auto it = grammar->lines[_part].rules.begin(); it != grammar->lines[_part].rules.end(); it++) {
        unsigned int _idx = it - grammar->lines[_part].rules.begin();
        unsigned int min_size = 0;
        if (parts.size() > 0) {
          auto min_elem_it = min_element(parts.begin(), parts.end(), [](auto a, auto b){ return a.size() < b.size(); });
          min_size = min_elem_it->size();
        }
        vector<vector<string>> reachables = find_starting_tokens(_part, _idx, limit - min_size);

        for (auto & _reachable : reachables) {
          for (auto & __part : parts) {
            vector<string> merged = __part;
            copy(_reachable.begin(), _reachable.end(), back_inserter(merged));
            new_parts.push_back(merged);
          }
        }
      }

      parts.clear();
      sort(new_parts.begin(), new_parts.end());
      unique_copy(new_parts.begin(), new_parts.end(), back_inserter(parts));
    }
  }

  return parts;
}

int LLAstBuilder::lookup(string rule_name, vector<string> tokens) {
  for (auto it = grammar->lines[rule_name].rules.begin(); it != grammar->lines[rule_name].rules.end(); it++) {
    unsigned int idx = it - grammar->lines[rule_name].rules.begin();
    vector<vector<string>> &lookup = rule_lookup[{rule_name, idx}];
    for (auto lookup_it = lookup.begin(); lookup_it != lookup.end(); lookup_it++) {
      unsigned int len = min(tokens.size(), lookup_it->size());

      if (len == 0) {
        cout << "LEN is zero!" << endl;
        exit(EXIT_FAILURE);
      }

      bool is_match = true;
      for (int i = 0; i < len; i++) {
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

void LLAstBuilder::print_rule_lookup() {
  cout << "Rule lookup matrix:\n";

  for (auto & rule_lookup_pair : rule_lookup) {
    for (auto & token_list : rule_lookup_pair.second) {
      cout << "[" << rule_lookup_pair.first.first << " #" << rule_lookup_pair.first.second << "]> ";
      copy(token_list.begin(), token_list.end(), ostream_iterator<string>(cout, " "));
      cout << endl;
    }
  }
}

bool LLAstBuilder::is_lookup_table_valid() {
  for (auto lhs_group_kv : rule_lookup) {
    for (auto lhs_list : lhs_group_kv.second) {

      for (int i = 0; i < grammar->lines[lhs_group_kv.first.first].rules.size(); i++) {
        // Same group - skip it.
        if (i == lhs_group_kv.first.second) continue;

        for (auto & rhs_list : rule_lookup[{lhs_group_kv.first.first, i}]) {
          if (lhs_list == rhs_list) {
            cout << "Collision in rules: ";
            cout << lhs_group_kv.first.first << " #" << lhs_group_kv.first.second << " and #" << i << endl;

            copy(lhs_list.begin(), lhs_list.end(), ostream_iterator<string>(cout, " "));
            cout << endl;
            copy(rhs_list.begin(), rhs_list.end(), ostream_iterator<string>(cout, " "));
            cout << endl;

            return false;
          }
        }
      }
    }
  }

  return true;
}
