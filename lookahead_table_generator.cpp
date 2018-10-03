#include "lookahead_table_generator.h"
#include "util.h"

#include <algorithm>
#include <vector>

LookaheadTableGenerator::LookaheadTableGenerator(Grammar* grammar,
                                                 unsigned int ll_max_level,
                                                 bool verbose_mode)
    : grammar(grammar),
      ll_max_level(ll_max_level),
      verbose_mode(verbose_mode){};

bool LookaheadTableGenerator::generate() {
  bool found_valid_lookup_table = false;
  for (unsigned int ll_level = 1; ll_level <= ll_max_level; ll_level++) {
    if (verbose_mode)
      cout << "Looking for LL(" << ll_level << ") lookup table.\n";

    build_lookup_table(ll_level);
    if (is_lookup_table_valid()) {
      found_valid_lookup_table = true;
      break;
    }
  }

  if (!found_valid_lookup_table) {
    if (verbose_mode)
      cout << "Error. No possible LL(" << ll_max_level << ") parser.\n";
    return false;
  }

  return true;
}

void LookaheadTableGenerator::build_lookup_table(unsigned int ll_level) {
  for (auto& grammar_line_pair : grammar->lines) {
    for (auto rule_it = grammar_line_pair.second->rules.begin();
         rule_it != grammar_line_pair.second->rules.end(); rule_it++) {
      unsigned int idx = rule_it - grammar_line_pair.second->rules.begin();

      rule_lookup[{grammar_line_pair.first, idx}] =
          find_starting_tokens(grammar_line_pair.first, idx, ll_level);
    }
  }

  if (verbose_mode) print_rule_lookup();
}

vector<vector<string>> LookaheadTableGenerator::find_starting_tokens(
    string rule_name, unsigned int idx, int limit) {
  vector<vector<string>> parts({{}});

  if (grammar->lines[rule_name]->rules[idx].parts.size() == 0) {
    parts[0].push_back(T_EMPTY);
    return parts;
  }

  for (auto& _part : grammar->lines[rule_name]->rules[idx].parts) {
    if (!any_of(parts.begin(), parts.end(),
                [&limit](auto p) { return p.size() < limit; }))
      continue;

    if (is_token(_part)) {
      transform(parts.begin(), parts.end(), parts.begin(),
                [&limit, &_part](auto p) {
                  if (p.size() >= limit) {
                    return p;
                  } else {
                    p.push_back(_part);
                    return p;
                  }
                });
    } else {
      vector<vector<string>> new_parts({});

      for (auto it = grammar->lines[_part]->rules.begin();
           it != grammar->lines[_part]->rules.end(); it++) {
        unsigned int _idx = it - grammar->lines[_part]->rules.begin();
        unsigned int min_size = 0;
        if (parts.size() > 0) {
          auto min_elem_it =
              min_element(parts.begin(), parts.end(),
                          [](auto a, auto b) { return a.size() < b.size(); });
          min_size = min_elem_it->size();
        }
        vector<vector<string>> reachables =
            find_starting_tokens(_part, _idx, limit - min_size);

        for (auto& _reachable : reachables) {
          for (auto& __part : parts) {
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

void LookaheadTableGenerator::print_rule_lookup() {
  cout << "Rule lookup matrix:\n";

  for (auto& rule_lookup_pair : rule_lookup) {
    for (auto& token_list : rule_lookup_pair.second) {
      cout << "[" << rule_lookup_pair.first.first << " #"
           << rule_lookup_pair.first.second << "]> ";
      copy(token_list.begin(), token_list.end(),
           ostream_iterator<string>(cout, " "));
      cout << endl;
    }
  }
}

bool LookaheadTableGenerator::is_lookup_table_valid() {
  for (auto lhs_group_kv : rule_lookup) {
    for (auto lhs_list : lhs_group_kv.second) {
      for (unsigned int i = 0;
           i < grammar->lines[lhs_group_kv.first.first]->rules.size(); i++) {
        // Same group - skip it.
        if (i == lhs_group_kv.first.second) continue;

        for (auto& rhs_list : rule_lookup[{lhs_group_kv.first.first, i}]) {
          if (lhs_list == rhs_list) {
            if (verbose_mode) {
              cout << "Collision in rules: ";
              cout << lhs_group_kv.first.first << " #"
                   << lhs_group_kv.first.second << " and #" << i << endl;
              copy(lhs_list.begin(), lhs_list.end(),
                   ostream_iterator<string>(cout, " "));
              cout << endl;
              copy(rhs_list.begin(), rhs_list.end(),
                   ostream_iterator<string>(cout, " "));
              cout << endl;
            }

            return false;
          }
        }
      }
    }
  }

  return true;
}
