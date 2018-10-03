#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "grammar_normalizer.h"
#include "grammar.h"
#include "grammar_rule.h"

using namespace std;

GrammarNormalizer::GrammarNormalizer(Grammar *g) : grammar(g) {}

void GrammarNormalizer::normalize() {
  eliminate_left_recursive();
  eliminate_premature_match();
}

void GrammarNormalizer::eliminate_left_recursive() {
  string left_rec_tag = get_left_recursive_tag();
  if (left_rec_tag.size() == 0) return;

  auto & rec_rules = grammar->lines[left_rec_tag]->rules;

  // Extract (and remove) left recursive rules.
  vector<GrammarRule> removed_rules;
  auto filter_fn = [left_rec_tag](GrammarRule &gr){ return gr.parts[0] == left_rec_tag; };
  copy_if(rec_rules.begin(), rec_rules.end(), back_inserter(removed_rules), filter_fn);
  rec_rules.erase(remove_if(rec_rules.begin(), rec_rules.end(), filter_fn));

  // Create new rule for indirection.
  string new_line_name(left_rec_tag);
  new_line_name.append("_X");
  grammar->lines[new_line_name] = make_unique<GrammarLine>();

  // Add proxies into old rule pointing to new rule.
  vector<GrammarRule> new_rules;
  transform(
    rec_rules.begin(),
    rec_rules.end(),
    back_inserter(new_rules),
    [new_line_name](GrammarRule &rule){
      GrammarRule new_rule(rule);
      new_rule.parts.push_back(new_line_name);
      return new_rule;
    }
  );
  copy(new_rules.begin(), new_rules.end(), back_inserter(rec_rules));

  // Fill up new rule.
  for (auto &removed_rule : removed_rules) {
    GrammarRule modified_removed_rule(removed_rule);
    modified_removed_rule.parts.erase(modified_removed_rule.parts.begin());
    grammar->lines[new_line_name]->rules.push_back(modified_removed_rule);

    modified_removed_rule.parts.push_back(new_line_name);
    grammar->lines[new_line_name]->rules.push_back(modified_removed_rule);
  }

  eliminate_left_recursive();
}

string GrammarNormalizer::get_left_recursive_tag() {
  for (auto it = grammar->lines.begin(); it != grammar->lines.end(); it++) {
    for (auto & rule : it->second->rules) {
      if (rule.parts.size() == 0) continue;

      if (rule.parts[0] == it->first) {
        return rule.parts[0];
      }
    }
  }
  return "";
}

void GrammarNormalizer::eliminate_premature_match() {
  for (auto & line : grammar->lines) {
    for (unsigned int i = 1; i < line.second->rules.size(); i++) {
      for (int j = i - 1; j >= 0; j--) {
        if (compare_rules(*(line.second->rules.begin() + i), *(line.second->rules.begin() + j)) == -1) {
          iter_swap(line.second->rules.begin() + i, line.second->rules.begin() + j);
        }
      }
    }
  }
}

int GrammarNormalizer::compare_rules(GrammarRule lhs, GrammarRule rhs) {
  for (unsigned int i = 0; i < min(lhs.parts.size(), rhs.parts.size()); i++) {
    if (lhs.parts[i] != rhs.parts[i]) return 1;
  }
  return lhs.parts.size() > rhs.parts.size() ? -1 : 1;
}
