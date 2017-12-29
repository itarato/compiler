#include "grammar.h"

#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

#include "grammar_line.h"
#include "grammar_rule.h"

Grammar::Grammar(istream_iterator<string> grammar_isi) {
    GrammarLine *grammar_line = nullptr;
    GrammarRule *grammar_rule = nullptr;
    string grammar_line_name;

    for (;;) {
        if (grammar_isi == istream_iterator<string>()) break;

        string word(*grammar_isi);

        if (is_line_def(word)) {
            if (grammar_line != nullptr) {
                grammar_line->add_rule(*grammar_rule);
                grammar_rule = new GrammarRule();
                save_grammar_line(grammar_line, grammar_line_name);
                grammar_line = nullptr;
            }
            word.pop_back();
            grammar_line = new GrammarLine();
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
    save_grammar_line(grammar_line, grammar_line_name);
    grammar_line = nullptr;
}

bool Grammar::is_line_def(string word) {
    return word.at(word.size() - 1) == ':';
}

bool Grammar::is_rule_divider(string word) { return word == "|"; }

void Grammar::save_grammar_line(GrammarLine *grammar_line, string name) {
    if (grammar_line == nullptr) return;
    lines[name] = *grammar_line;
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
