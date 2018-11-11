#include "grammar.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "grammar_line.h"
#include "grammar_rule.h"

Grammar::Grammar(istream_iterator<string> grammar_isi) {
  unique_ptr<GrammarLine> grammar_line;
  GrammarRule *grammar_rule = nullptr;
  string grammar_line_name;

  for (;;) {
    if (grammar_isi == istream_iterator<string>()) break;

    string word(*grammar_isi);

    if (is_line_def(word)) {
      if (grammar_line) {
        grammar_line->add_rule(*grammar_rule);
        save_grammar_line(forward<unique_ptr<GrammarLine>>(grammar_line),
                          grammar_line_name);
      }
      word.pop_back();
      grammar_line = make_unique<GrammarLine>();
      grammar_rule = new GrammarRule();
      grammar_line_name = word;
    } else if (is_rule_divider(word)) {
      grammar_line->add_rule(*grammar_rule);
      grammar_rule = new GrammarRule();
    } else {
      grammar_rule->add_part(word);
    }

    grammar_isi++;
  }
  grammar_line->add_rule(*grammar_rule);
  save_grammar_line(forward<unique_ptr<GrammarLine>>(grammar_line),
                    grammar_line_name);
}

bool Grammar::is_line_def(string word) {
  return word.at(word.size() - 1) == ':';
}

bool Grammar::is_rule_divider(string word) { return word == "|"; }

void Grammar::save_grammar_line(unique_ptr<GrammarLine> grammar_line,
                                string name) {
  if (!grammar_line) return;
  grammar_line.swap(lines[name]);
}

vector<pair<string, GrammarRule>> Grammar::flatten() {
  vector<pair<string, GrammarRule>> flat_rules{};

  for (const auto &line_pair : lines) {
    for (const auto &rule : line_pair.second->rules) {
      flat_rules.push_back({line_pair.first, rule});
    }
  }

  return flat_rules;
}

Grammar new_grammar_from_filename(char *filename) {
  ifstream ifs(filename);
  istream_iterator<string> isit(ifs);
  return Grammar(isit);
}

Grammar new_grammar_from_string(string raw) {
  istringstream iss(raw);
  istream_iterator<string> isit(iss);
  return Grammar(isit);
}
