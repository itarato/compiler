#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <cstdio>
#include <cstdint>

#define RULE_T_NAME "T_NAME"
#define RULE_T_NUMBER "T_NUMBER"
#define RULE_T_ADD "T_ADD"
#define RULE_T_SUB "T_SUB"
#define RULE_T_MUL "T_MUL"
#define RULE_T_DIV "T_DIV"
#define RULE_T_EOP "T_EOP"

using namespace std;

class Token;

typedef vector<Token>::iterator vec_iter_t;

class GrammarRule {
public:
    vector<string> parts;
    bool is_one_or_more;

    void add_part(string part) { parts.push_back(part); };

    friend ostream & operator<<(ostream & os, const GrammarRule & self) {
        if (self.is_one_or_more) os << "+";
        os << "{ ";
        copy(self.parts.begin(), self.parts.end(), ostream_iterator<string>(os, " "));
        os << "}";
        return os;
    };

    void set_one_or_more() { is_one_or_more = true; }
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
                if (word == "+") {
                    grammar_rule->set_one_or_more();
                } else {
                    grammar_rule->add_part(word);
                }
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

enum TokenType {
    NAME,
    NUMBER,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    SEMICOLON,
    EOP,
};

ostream & operator<<(ostream & os, const TokenType & self) {
    switch (self) {
        case NAME:
            os << "NAME";
            break;
        case NUMBER:
            os << "NUMBER";
            break;
        case OP_ADD:
            os << "OP_ADD";
            break;
        case OP_SUB:
            os << "OP_SUB";
            break;
        case OP_MUL:
            os << "OP_MUL";
            break;
        case OP_DIV:
            os << "OP_DIV";
            break;
        case SEMICOLON:
            os << "SEMICOLON";
            break;
        case EOP:
            os << "EOP";
            break;
    }
    return os;
}

class Token {
public:
    TokenType type;
    string value;

    Token(TokenType type, string value) : type(type), value(value) {}; 

    friend ostream & operator<<(ostream & os, const Token & self) {
        os << self.type << "(" << self.value << ")";
        return os;
    }
};

class Tokenizer {
public:
    vector<Token> tokens;

    Tokenizer() {
        ifstream source_if("sample.p");
      
        for (;;) {
            for (char top_ch = source_if.peek(); is_whitespace(source_if.peek()); source_if.get()) {}

            if (source_if.peek() == EOF) break;

            tokens.push_back(get_token(&source_if));
        }

        tokens.push_back({TokenType::EOP, ""});
    };

    friend ostream & operator<<(ostream & os, const Tokenizer & self) {
        os << "T[ ";
        for (const auto token : self.tokens) {
            os << token << " ";
        }
        os << "]";
        return os;
    };

private:
    Token get_token(ifstream *source_if) {
        char ch = source_if->peek();
        if (is_numeric(ch)) {
            return read_number_token(source_if);
        } else if (is_letter(ch)) {
            return read_name_token(source_if);
        } else if (is_op(ch)) {
            source_if->get();
            return read_op_token(ch);
        } else if (is_semicolon(ch)) {
            source_if->get();
            return Token(TokenType::SEMICOLON, string{ch});
        } else {
            cout << "Unexpected char: " << (int) ch << endl;
            exit(EXIT_FAILURE);
        }
    };

    Token read_number_token(ifstream *source_if) {
        string value;
        while (is_numeric(source_if->peek())) {
            value.push_back(source_if->get());
        }
        return Token(TokenType::NUMBER, value);
    };

    Token read_name_token(ifstream *source_if) {
        string value;
        while (is_letter(source_if->peek())) {
            value.push_back(source_if->get());
        }
        return Token(TokenType::NAME, value);
    };

    Token read_op_token(char ch) {
        switch (ch) {
            case '+':
                return Token(TokenType::OP_ADD, string{ch});
            case '-':
                return Token(TokenType::OP_SUB, string{ch});
            case '*':
                return Token(TokenType::OP_MUL, string{ch});
            case '/':
                return Token(TokenType::OP_DIV, string{ch});
            default:
                cout << "Unknown OP: " << ch << endl;
                exit(EXIT_FAILURE);
        }
    };

    bool is_numeric(char ch) { return ch >= '0' && ch <= '9'; };
    bool is_letter(char ch) { return ch >= 'a' && ch <= 'z'; };
    bool is_op(char ch) { return ch == '+' || ch == '-' || ch == '*' || ch == '/'; };
    bool is_whitespace(char ch) { return !(is_numeric(ch) || is_letter(ch) || is_op(ch) || is_semicolon(ch) || ch == EOF); };
    bool is_semicolon(char ch) { return ch == ';'; }
};

struct SuccessWithPos {
public:
    bool is_success;
    uint8_t pos;
};

class AstBuilder {
public:
    Grammar *grammar;
    Tokenizer *tokenizer;

    AstBuilder(Grammar *grammar, Tokenizer *tokenizer) : grammar(grammar), tokenizer(tokenizer) {};

    bool build() {
        cout << "BUILD" << endl;
        vec_iter_t it = tokenizer->tokens.begin();
        SuccessWithPos success_with_pos = try_grammar_line(&(grammar->lines["PROG"]), &it);
        return success_with_pos.is_success;
    };

private:
    SuccessWithPos try_grammar_line(GrammarLine *gr_line, vec_iter_t *token_it) {
        cout << "Try grammar line: " << *gr_line << " on: " << **token_it << endl;
        auto orig_token_it = *token_it;

        uint8_t idx = 0;
        for (auto rule : gr_line->rules) {
            if (try_grammar_rule(&rule, token_it)) {
                return {true, idx};
            }
            *token_it = orig_token_it;
            idx++;
        }

        return {false, 0};
    };

    bool try_grammar_rule(GrammarRule *rule, vec_iter_t *token_it) {
        cout << "- Try grammar rule: " << *rule << " on: " << **token_it << endl;
        auto orig_token_it = *token_it;

        for (const auto rule_part : rule->parts) {
            if (is_token(rule_part)) {
                if (is_token_match(rule_part, **token_it)) {
                    (*token_it)++;
                } else {
                    *token_it = orig_token_it;
                    return false;
                }
            } else {
                SuccessWithPos line_success_with_pos = try_grammar_line(&grammar->lines[rule_part], token_it);
                if (!line_success_with_pos.is_success) {
                    *token_it = orig_token_it;
                    return false;
                }
            }
        }

        return true;
    }

    bool is_token(string rule_part) {
        return rule_part.at(0) == 'T' && rule_part.at(1) == '_';
    }

    bool is_token_match(string rule_part, Token token) {
        return 
            (rule_part == RULE_T_NAME && token.type == TokenType::NAME) ||
            (rule_part == RULE_T_NUMBER && token.type == TokenType::NUMBER) ||
            (rule_part == RULE_T_ADD && token.type == TokenType::OP_ADD) ||
            (rule_part == RULE_T_SUB && token.type == TokenType::OP_SUB) ||
            (rule_part == RULE_T_MUL && token.type == TokenType::OP_MUL) ||
            (rule_part == RULE_T_DIV && token.type == TokenType::OP_DIV);
    }
};

int main() {
    Grammar g;
    cout << g << endl;

    Tokenizer t;
    cout << t << endl;

    AstBuilder ab(&g, &t);
    ab.build();

    exit(EXIT_SUCCESS);
}
