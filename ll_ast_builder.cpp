#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>

#include "ast_node.h"
#include "ll_ast_builder.h"
#include "token.h"
#include "util.h"

void print_flat_grammar_rules(vector<FlatGrammarRule> fgr) {
    cout << "Grammar / decision list:\n";
    unsigned int idx = 0;
    for (auto &rule : fgr) {
        cout << "#" << idx++ << " " << rule.rule_name << " :> " << rule.rule
             << endl;
    }
}

LLAstBuilder::LLAstBuilder(Grammar *g, Tokenizer *t)
    : grammar(g), tokenizer(t) {
    build_flat_grammar_version();
};

AstNode *LLAstBuilder::build() {
    print_flat_grammar_rules(flat_grammar);

    vector<Token> source_tokens;
    copy(tokenizer->tokens.rbegin(), tokenizer->tokens.rend(),
         back_inserter(source_tokens));
    vector<string> rule_tokens({START_RULE});

    AstNode *p_ast_node = new AstNode(START_RULE);

    while (!rule_tokens.empty() && !source_tokens.empty()) {
        cout << "\x1B[92mR\\\x1B[0m ";
        copy(rule_tokens.rbegin(), rule_tokens.rend(),
             ostream_iterator<string>(cout, " "));
        cout << endl;
        cout << "\x1B[92mS/\x1B[0m ";
        for (auto it = source_tokens.rbegin(); it != source_tokens.rend(); it++)
            cout << token_e_to_s(it->type) << " ";
        cout << endl << endl;

        if (token_eq(rule_tokens.back(), source_tokens.back().type)) {
            cout << "Match - deterministic tokens: " << rule_tokens.back()
                 << endl
                 << endl;

            rule_tokens.pop_back();
            source_tokens.pop_back();
        } else {
            // If rule is deterministic and not matching (prev cond).
            if (is_token(rule_tokens.back())) {
                cout << "Non matching deterministic token.\n";
                exit(EXIT_FAILURE);
            }

            // From the current first rule the deterministic source token is
            // unreachable.
            string source_token_string =
                token_e_to_s(source_tokens.back().type);
            if (rule_lookup[rule_tokens.back()].find(source_token_string) ==
                rule_lookup[rule_tokens.back()].end()) {
                if (rule_lookup[rule_tokens.back()].find(T_EMPTY) ==
                    rule_lookup[rule_tokens.back()].end()) {
                    cout << "No matching rule.\n";
                    exit(EXIT_FAILURE);
                }
                source_token_string = T_EMPTY;
            }

            unsigned int rule_idx =
                rule_lookup[rule_tokens.back()][source_token_string];
            cout << "Expand rule: #" << rule_idx << endl << endl;

            rule_tokens.pop_back();
            copy(flat_grammar[rule_idx].rule.parts.rbegin(),
                 flat_grammar[rule_idx].rule.parts.rend(),
                 back_inserter(rule_tokens));
        }
    }

    return p_ast_node;
}

void LLAstBuilder::build_flat_grammar_version() {
    unsigned int idx = 0;
    for (auto &line_pair : grammar->lines) {
        string rule_name(line_pair.first);
        for (auto &rule : line_pair.second.rules) {
            flat_grammar.push_back({rule_name, rule});
            vector<string> reached_tokens = find_starting_tokens(rule);
            for (auto &reached_token : reached_tokens) {
                // Validate grammar - LL1 is not that strong - easy to have
                // multiple nonterminals for terminals.
                if (rule_lookup.find(rule_name) != rule_lookup.end() &&
                    rule_lookup[rule_name].find(reached_token) !=
                        rule_lookup[rule_name].end()) {
                    cout << "Invalid grammar - terminal token can be reached "
                            "from multiple non terminal.\n";
                    cout << "Collision: " << rule_name << " - " << reached_token
                         << " : " << rule_lookup[rule_name][reached_token]
                         << " vs " << idx << " in\n";
                    print_flat_grammar_rules(flat_grammar);
                    exit(EXIT_FAILURE);
                }

                rule_lookup[rule_name][reached_token] = idx;
            }

            idx++;
        }
    }

    print_rule_lookup();
}

void LLAstBuilder::print_rule_lookup() {
    cout << "Rule lookup matrix:\n";

    for (auto &rule_lookup_pair : rule_lookup) {
        for (auto &token_pair : rule_lookup_pair.second) {
            cout << "[" << rule_lookup_pair.first << ", " << token_pair.first
                 << "] -> " << token_pair.second << endl;
        }
    }
}

vector<string> LLAstBuilder::find_starting_tokens(GrammarRule rule) {
    if (rule.parts.size() == 0) return {T_EMPTY};

    vector<string> out;
    if (is_token(rule.parts[0])) {
        out.push_back(rule.parts[0]);
    } else {
        for (auto &rule : grammar->lines[rule.parts[0]].rules) {
            auto sub_out = find_starting_tokens(rule);
            copy(sub_out.begin(), sub_out.end(), back_inserter(out));
        }
    }

    return out;
}
