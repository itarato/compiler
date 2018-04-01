#pragma once

#include <map>
#include <string>
#include <vector>

#include "ast_node.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "token.h"
#include "tokenizer.h"

using namespace std;

struct FlatGrammarRule {
    string rule_name;
    GrammarRule rule;
};

void print_flat_grammar_rules(vector<FlatGrammarRule>);

class LLAstBuilder {
   public:
    Grammar *grammar;
    Tokenizer *tokenizer;
    vector<FlatGrammarRule> flat_grammar;
    map<string, map<unsigned int, vector<string>>> rule_lookup;

    LLAstBuilder(Grammar *, Tokenizer *);
    AstNode *build();

   private:
    void build_flat_grammar_version();
    vector<vector<string>> find_starting_tokens(GrammarRule);
    void print_rule_lookup();
    int lookup(string, vector<string>);
};
