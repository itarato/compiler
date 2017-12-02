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
  string left_rec_tag = get_left_recursive_tag();
  if (left_rec_tag.size() == 0) return;

  auto & rec_rules = grammar->lines[left_rec_tag].rules;

  // Extract (and remove) left recursive rules.
  vector<GrammarRule> removed_rules;
  auto filter_fn = [left_rec_tag](GrammarRule &gr){ return gr.parts[0] == left_rec_tag; };
  copy_if(rec_rules.begin(), rec_rules.end(), back_inserter(removed_rules), filter_fn);
  rec_rules.erase(remove_if(rec_rules.begin(), rec_rules.end(), filter_fn));

  // Create new rule for indirection.
  string new_line_name(left_rec_tag);
  new_line_name.append("_X");
  grammar->lines[new_line_name] = GrammarLine();

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
    grammar->lines[new_line_name].rules.push_back(modified_removed_rule);

    modified_removed_rule.parts.push_back(new_line_name);
    grammar->lines[new_line_name].rules.push_back(modified_removed_rule);
  }

  normalize();
}

string GrammarNormalizer::get_left_recursive_tag() {
  for (auto it = grammar->lines.begin(); it != grammar->lines.end(); it++) {
    for (auto & rule : it->second.rules) {
      if (rule.parts.size() == 0) continue;

      if (rule.parts[0] == it->first) {
        return rule.parts[0];
      }
    }
  }
  return "";
}
