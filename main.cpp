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
#define RULE_T_LT "T_LT"
#define RULE_T_GT "T_GT"
#define RULE_T_EOP "T_EOP"
#define RULE_T_SEMICOLON "T_SEMICOLON"
#define RULE_T_BRACE_OPEN "T_BRACE_OPEN"
#define RULE_T_BRACE_CLOSE "T_BRACE_CLOSE"
#define RULE_T_KW_IF "T_KW_IF"

using namespace std;

uint8_t indent = 0;

void add_indent() {
  for (uint8_t i = 0; i < indent; i++) cout << "\e[90m| \e[0m";
}

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
  OP_LT,
  OP_GT,
  SEMICOLON,
  EOP,
  KW_IF,
  BRACE_OPEN,
  BRACE_CLOSE,
};

ostream & operator<<(ostream & os, const TokenType & self) {
  switch (self) {
    case NAME: os << "NAME"; break;
    case NUMBER: os << "NUMBER"; break;
    case OP_ADD: os << "OP_ADD"; break;
    case OP_SUB: os << "OP_SUB"; break;
    case OP_MUL: os << "OP_MUL"; break;
    case OP_DIV: os << "OP_DIV"; break;
    case OP_LT: os << "OP_LT"; break;
    case OP_GT: os << "OP_GT"; break;
    case SEMICOLON: os << "SEMICOLON"; break;
    case EOP: os << "EOP"; break;
    case KW_IF: os << "KW_IF"; break;
    case BRACE_OPEN: os << "BRACE_OPEN"; break;
    case BRACE_CLOSE: os << "BRACE_CLOSE"; break;
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
    } else if (is_op(ch)) {
      source_if->get();
      return read_op_token(ch);
    } else if (ch == ';') {
      source_if->get();
      return Token(TokenType::SEMICOLON, string{ch});
    } else if (ch == '{') {
      source_if->get();
      return Token(TokenType::BRACE_OPEN, string{ch});
    } else if (ch == '}') {
      source_if->get();
      return Token(TokenType::BRACE_CLOSE, string{ch});
    } else if (is_letter(ch)) {
      string s = read_char_list_token(source_if);
      if (is_keyword(s)) {
        return wrap_keyword(s);
      } else {
        return Token(TokenType::NAME, s);
      }
    } else {
      cout << "Unexpected char: " << (int) ch << endl;
      exit(EXIT_FAILURE);
    }
  };

  bool is_keyword(string s) {
    return s == "if";
  }

  Token wrap_keyword(string s) {
    if (s == "if") {
      return Token(TokenType::KW_IF, s);
    }
    exit(EXIT_FAILURE);
  }

  Token read_number_token(ifstream *source_if) {
    string value;
    while (is_numeric(source_if->peek())) {
      value.push_back(source_if->get());
    }
    return Token(TokenType::NUMBER, value);
  };

  string read_char_list_token(ifstream *source_if) {
    string value;
    while (is_letter(source_if->peek())) {
      value.push_back(source_if->get());
    }
    return value;
  };

  Token read_op_token(char ch) {
    switch (ch) {
      case '+': return Token(TokenType::OP_ADD, string{ch});
      case '-': return Token(TokenType::OP_SUB, string{ch});
      case '*': return Token(TokenType::OP_MUL, string{ch});
      case '/': return Token(TokenType::OP_DIV, string{ch});
      case '<': return Token(TokenType::OP_LT, string{ch});
      case '>': return Token(TokenType::OP_GT, string{ch});
      default:
        cout << "Unknown OP: " << ch << endl;
        exit(EXIT_FAILURE);
    }
  };

  bool is_numeric(char ch) { return ch >= '0' && ch <= '9'; };
  bool is_letter(char ch) { return ch >= 'a' && ch <= 'z'; };
  bool is_op(char ch) { return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '>'; };
  bool is_whitespace(char ch) {
    return !(
      is_numeric(ch) ||
      is_letter(ch) ||
      is_op(ch) ||
      ch == ';' ||
      ch == EOF ||
      ch == '}' ||
      ch == '{');
  };
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
    indent++;

    add_indent();
    cout << "Try line: \e[96m" << *gr_line << "\e[0mon: \e[96m" << **token_it << "\e[0m" << endl;
    auto orig_token_it = *token_it;

    uint8_t idx = 0;
    for (auto rule : gr_line->rules) {
      if (try_grammar_rule(&rule, token_it)) {
        add_indent();
        cout << "Y" << endl;
        indent--;
        return {true, idx};
      }
      add_indent();
      cout << "Revert  \e[91m" << **token_it << "\e[0m -> \e[91m" << *orig_token_it << "\e[0m" << endl;
      *token_it = orig_token_it;
      idx++;
    }

    add_indent();
    cout << "N" << endl;
    indent--;
    return {false, 0};
  };

  bool try_grammar_rule(GrammarRule *rule, vec_iter_t *token_it) {
    indent++;

    add_indent();
    cout << "Try rule: \e[93m" << *rule << "\e[0m on: \e[93m" << **token_it << "\e[0m" << endl;

    for (const auto rule_part : rule->parts) {
      if (is_token(rule_part)) {
        if (is_token_match(rule_part, **token_it)) {
          add_indent();
          cout << "MATCH \e[32m" << **token_it << "\e[0m" << endl;
          (*token_it)++;
        } else {
          add_indent();
          cout << "N" << endl;
          indent--;
          return false;
        }
      } else {
        SuccessWithPos line_success_with_pos = try_grammar_line(&grammar->lines[rule_part], token_it);
        if (!line_success_with_pos.is_success) {
          add_indent();
          cout << "N" << endl;
          indent--;
          return false;
        }
      }
    }

    add_indent();
    cout << "Y" << endl;
    indent--;
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
      (rule_part == RULE_T_DIV && token.type == TokenType::OP_DIV) ||
      (rule_part == RULE_T_LT && token.type == TokenType::OP_LT) ||
      (rule_part == RULE_T_GT && token.type == TokenType::OP_GT) ||
      (rule_part == RULE_T_SEMICOLON && token.type == TokenType::SEMICOLON) ||
      (rule_part == RULE_T_EOP && token.type == TokenType::EOP) ||
      (rule_part == RULE_T_BRACE_OPEN && token.type == TokenType::BRACE_OPEN) ||
      (rule_part == RULE_T_BRACE_CLOSE && token.type == TokenType::BRACE_CLOSE) ||
      (rule_part == RULE_T_KW_IF && token.type == TokenType::KW_IF);
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
