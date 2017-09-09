#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>

using namespace std;

class GrammarRule {
public:
    vector<string> parts;
    void add_part(string part) { parts.push_back(part); };

    friend ostream & operator<<(ostream & os, const GrammarRule & self) {
        os << "{ ";
        copy(self.parts.begin(), self.parts.end(), ostream_iterator<string>(os, " "));
        os << "}";
        return os;
    };
};

class GrammarLine {
public:
    vector<GrammarRule> rules;
    GrammarLine() {};
    void add_rule(GrammarRule grammar_rule) { rules.push_back(grammar_rule); };

    friend ostream & operator<<(ostream & os, const GrammarLine & self) {
        for (auto const rule : self.rules) {
            os << rule << " ";
        }
        return os;
    };
};

class Grammar {
public:
    map<string, GrammarLine> lines;

    Grammar() {
        ifstream grammar_ifs("grammar");
        
        GrammarLine *grammar_line = nullptr;
        GrammarRule *grammar_rule = nullptr;
        string grammar_line_name;

        for (;;) {
            string word;

            grammar_ifs >> word;
            if (word.size() == 0) break;

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
        }
        grammar_line->add_rule(*grammar_rule);
        save_grammar_line(grammar_line, grammar_line_name);
        grammar_line = nullptr;
    };

    bool is_line_def(string word) {
        return word.at(word.size() - 1) == ':';
    };

    bool is_rule_divider(string word) {
        return word == "|";
    };

    void save_grammar_line(GrammarLine *grammar_line, string name) {
        if (grammar_line == nullptr) return;
        lines[name] = *grammar_line;
    };

    friend ostream & operator<<(ostream & os, const Grammar & self) {
        for (auto const line : self.lines) {
            os << line.first << ": " << line.second << endl;
        }
        return os;
    };
};

int main() {
    Grammar g;
    cout << g << endl;

    exit(EXIT_SUCCESS);
}
