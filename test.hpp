#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "ast_builder.h"
#include "ast_node.h"
#include "grammar.h"
#include "grammar_normalizer.h"
#include "ll_ast_builder.h"
#include "tokenizer.h"

using namespace std;

#define ASSERT(t) assert((t), __FUNCTION__, __LINE__)
#define ASSERT_EQUAL(lhs, rhs) assert_eq(lhs, rhs, __FUNCTION__, __LINE__)

class Test {
public:
  unsigned int success_count;
  unsigned int failure_count;

  Test() : success_count(0), failure_count(0) { cout << "TEST" << endl; };

  void run() {
    test_grammar_basic();
    test_grammar_multiline();
    test_grammar_multiple_rules();
    test_grammar_empty_rule();
    test_grammar_multiple_rule_parts();

    test_tokenizer_end_of_program_is_added_always();
    test_tokenizer_source_can_end_empty_or_newline();
    test_tokenizer_ignores_whitespaces();
    test_tokenizer_recognize_basic_token_types();
    test_tokenizer_recognize_strings();
    test_tokenizer_recognize_multiple_tokens();

    test_ast_builder_examples();
    test_ast_builder_empty();
    test_ast_builder_same_rule_beginning();

    test_grammar_normalizer_keeps_correct_grammar_as_is();
    test_grammar_normalizer_fix_left_recursion();
    test_grammar_normalizer_sort_rules_to_avoid_premature_match();

    test_ll_ast_builder_examples();
    test_ll_ast_builder_one_lookahead();

    cout << endl
         << "\x1B[1mCOMPLETE\x1B[0m - \x1B[92mSUCCESS: " << success_count
         << "\x1B[0m \x1B[91mFAILURE: " << failure_count << "\x1B[0m" << endl
         << endl;
  };

  // GRAMMAR TESTS //////////////////////////////////////////////////////////

  void test_grammar_basic() {
    Grammar g(new_grammar_from_string("PROG: T_EOF"));

    ASSERT_EQUAL((size_t)1, g.lines.size());
    ASSERT(g.lines.find("PROG") != g.lines.end());
    ASSERT_EQUAL((size_t)1, g.lines["PROG"].rules.size());
    ASSERT_EQUAL((size_t)1, g.lines["PROG"].rules[0].parts.size());
    ASSERT_EQUAL((string) "T_EOF", g.lines["PROG"].rules[0].parts[0]);
  };

  void test_grammar_multiline() {
    Grammar g(new_grammar_from_string("A: B\nB: T_C"));

    ASSERT_EQUAL((size_t)2, g.lines.size());
  }

  void test_grammar_multiple_rules() {
    Grammar g(new_grammar_from_string("A: B | C | D"));

    ASSERT_EQUAL((size_t)3, g.lines["A"].rules.size());
  }

  void test_grammar_empty_rule() {
    Grammar g1(new_grammar_from_string("A: "));
    ASSERT_EQUAL((size_t)0, g1.lines["A"].rules[0].parts.size());

    Grammar g2(new_grammar_from_string("A: B |"));
    ASSERT_EQUAL((size_t)0, g2.lines["A"].rules[1].parts.size());

    Grammar g3(new_grammar_from_string("A: B | | C"));
    ASSERT_EQUAL((size_t)0, g3.lines["A"].rules[1].parts.size());
  }

  void test_grammar_multiple_rule_parts() {
    Grammar g(new_grammar_from_string("A: B C D"));

    ASSERT_EQUAL((size_t)3, g.lines["A"].rules[0].parts.size());
  }

  // TOKENIZER TESTS ////////////////////////////////////////////////////////

  void test_tokenizer_end_of_program_is_added_always() {
    Tokenizer t1(new_tokenizer_from_string(""));

    ASSERT_EQUAL((size_t)1, t1.tokens.size());
    ASSERT_EQUAL(TokenType::EOP, t1.tokens[0].type);

    Tokenizer t2(new_tokenizer_from_string("puts"));

    ASSERT_EQUAL((size_t)2, t2.tokens.size());
    ASSERT_EQUAL(TokenType::EOP, t2.tokens[1].type);
  }

  void test_tokenizer_source_can_end_empty_or_newline() {
    vector<string> test_cases{"puts", "puts ", "puts\n", "puts \t \n \n \r"};

    for (const auto &test_case : test_cases) {
      Tokenizer t(new_tokenizer_from_string(test_case));
      ASSERT_EQUAL((size_t)2, t.tokens.size());
      ASSERT_EQUAL(TokenType::NAME, t.tokens[0].type);
    }
  }

  void test_tokenizer_ignores_whitespaces() {
    vector<string> test_cases{"puts(", "puts (", "puts \t \n ("};

    for (const auto &test_case : test_cases) {
      Tokenizer t(new_tokenizer_from_string(test_case));
      ASSERT_EQUAL((size_t)3, t.tokens.size());
    }
  }

  void test_tokenizer_recognize_basic_token_types() {
    test_tokenizer_recognize_type(TokenType::NAME, {"puts", "foo123", "b"});
    test_tokenizer_recognize_type(TokenType::NUMBER, {"1", "0", "123"});
    test_tokenizer_recognize_type(TokenType::OP_ADD, {"+"});
    test_tokenizer_recognize_type(TokenType::OP_SUB, {"-"});
    test_tokenizer_recognize_type(TokenType::OP_MUL, {"*"});
    test_tokenizer_recognize_type(TokenType::OP_DIV, {"/"});
    test_tokenizer_recognize_type(TokenType::OP_GT, {">"});
    test_tokenizer_recognize_type(TokenType::OP_LT, {"<"});
    test_tokenizer_recognize_type(TokenType::ASSIGN, {"="});
    test_tokenizer_recognize_type(TokenType::COMMA, {","});
    test_tokenizer_recognize_type(TokenType::SEMICOLON, {";"});
    test_tokenizer_recognize_type(TokenType::KW_IF, {"if"});
    test_tokenizer_recognize_type(TokenType::KW_WHILE, {"while"});
    test_tokenizer_recognize_type(TokenType::KW_FOR, {"for"});
    test_tokenizer_recognize_type(TokenType::KW_FN, {"fn"});
    test_tokenizer_recognize_type(TokenType::BRACE_OPEN, {"{"});
    test_tokenizer_recognize_type(TokenType::BRACE_CLOSE, {"}"});
    test_tokenizer_recognize_type(TokenType::PAREN_OPEN, {"("});
    test_tokenizer_recognize_type(TokenType::PAREN_CLOSE, {")"});
  }

  void test_tokenizer_recognize_strings() {
    Tokenizer t1(new_tokenizer_from_string("\"fn 'while' puts foobar 123\""));
    ASSERT_EQUAL((string) "fn 'while' puts foobar 123", t1.tokens[0].value);
    ASSERT_EQUAL(TokenType::STRING, t1.tokens[0].type);

    Tokenizer t2(new_tokenizer_from_string("'fn \"while\" puts foobar 123'"));
    ASSERT_EQUAL((string) "fn \"while\" puts foobar 123", t2.tokens[0].value);
    ASSERT_EQUAL(TokenType::STRING, t2.tokens[0].type);
  }

  void test_tokenizer_recognize_multiple_tokens() {
    Tokenizer t(new_tokenizer_from_string("if (a < b) { puts 'hello'; }"));
    ASSERT_EQUAL((size_t)12, t.tokens.size());
  }

  // AST BUILDER TESTS //////////////////////////////////////////////////////

  void test_ast_builder_examples() {
    ASSERT_EQUAL((string) "{\"PROG\":[\"T_NUMBER(123)\",\"T_EOP()\"]}",
                 ast_node_to_json(build_ast("PROG: T_NUMBER T_EOP", "123")));

    string expr_grammar = "PROG: EXPR T_EOP\nEXPR: T_NUMBER | T_PAREN_OPEN "
                          "EXPR OP EXPR T_PAREN_CLOSE\nOP: T_ADD";
    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"EXPR\":[\"T_NUMBER(1)\"]},\"T_EOP()\"]}",
        ast_node_to_json(build_ast(expr_grammar, "1")));

    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"EXPR\":[\"T_PAREN_OPEN(()\",{\"EXPR\":[\"T_"
                 "NUMBER(1)\"]},{\"OP\":[\"T_ADD(+)\"]},{\"EXPR\":[\"T_NUMBER("
                 "2)\"]},\"T_PAREN_CLOSE())\"]},\"T_EOP()\"]}",
        ast_node_to_json(build_ast(expr_grammar, "(1 + 2)")));

    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"EXPR\":[\"T_PAREN_OPEN(()\",{\"EXPR\":[\"T_"
                 "NUMBER(1)\"]},{\"OP\":[\"T_ADD(+)\"]},{\"EXPR\":[\"T_PAREN_"
                 "OPEN(()\",{\"EXPR\":[\"T_NUMBER(2)\"]},{\"OP\":[\"T_ADD(+)\"]"
                 "},{\"EXPR\":[\"T_NUMBER(3)\"]},\"T_PAREN_CLOSE())\"]},\"T_"
                 "PAREN_CLOSE())\"]},\"T_EOP()\"]}",
        ast_node_to_json(build_ast(expr_grammar, "(1 + (2 + 3))")));

    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"EXPR\":[\"T_PAREN_OPEN(()\",{\"EXPR\":[\"T_"
                 "PAREN_OPEN(()\",{\"EXPR\":[\"T_NUMBER(1)\"]},{\"OP\":[\"T_"
                 "ADD(+)\"]},{\"EXPR\":[\"T_NUMBER(2)\"]},\"T_PAREN_CLOSE())\"]"
                 "},{\"OP\":[\"T_ADD(+)\"]},{\"EXPR\":[\"T_NUMBER(3)\"]},\"T_"
                 "PAREN_CLOSE())\"]},\"T_EOP()\"]}",
        ast_node_to_json(build_ast(expr_grammar, "((1 + 2) + 3)")));

    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"EXPR\":[\"T_PAREN_OPEN(()\",{\"EXPR\":[\"T_"
                 "PAREN_OPEN(()\",{\"EXPR\":[\"T_NUMBER(1)\"]},{\"OP\":[\"T_"
                 "ADD(+)\"]},{\"EXPR\":[\"T_NUMBER(2)\"]},\"T_PAREN_CLOSE())\"]"
                 "},{\"OP\":[\"T_ADD(+)\"]},{\"EXPR\":[\"T_PAREN_OPEN(()\",{"
                 "\"EXPR\":[\"T_NUMBER(3)\"]},{\"OP\":[\"T_ADD(+)\"]},{"
                 "\"EXPR\":[\"T_NUMBER(4)\"]},\"T_PAREN_CLOSE())\"]},\"T_PAREN_"
                 "CLOSE())\"]},\"T_EOP()\"]}",
        ast_node_to_json(build_ast(expr_grammar, "((1 + 2) + (3 + 4))")));
  }

  void test_ast_builder_empty() {
    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"EXPR\":[]},\"T_EOP()\"]}",
        ast_node_to_json(build_ast("PROG: EXPR T_EOP\nEXPR: T_NUMBER |", "")));
  }

  void test_ast_builder_same_rule_beginning() {
    ASSERT_EQUAL(
        (string) "{\"PROG\":[{\"A\":[{\"B\":[\"T_ADD(+)\"]},\"T_NUMBER(12)\"]},"
                 "\"T_EOP()\"]}",
        ast_node_to_json(build_ast(
            "PROG: A T_EOP\nA: B T_NUMBER | B T_STRING\nB: T_ADD | T_SUB",
            "+ 12")));
  }

  // GRAMMAR NORMALIZER /////////////////////////////////////////////////////

  void test_grammar_normalizer_keeps_correct_grammar_as_is() {
    test_grammar_normalizer_expect("A: B | C \nB: T1 | T2 \nC: T3 | T4 \n",
                                   "A: B | C \nB: T1 | T2 \nC: T3 | T4 \n");
  }

  void test_grammar_normalizer_fix_left_recursion() {
    test_grammar_normalizer_expect(
        "A: A B | C \nB: T1 | T2 \nC: T3 | T4 ",
        "A: C A_X | C \nA_X: B A_X | B \nB: T1 | T2 \nC: T3 | T4 \n");
  }

  void test_grammar_normalizer_sort_rules_to_avoid_premature_match() {
    test_grammar_normalizer_expect("A: B | B C \n", "A: B C | B \n");
  }

  // LL PARSER //////////////////////////////////////////////////////////////

  void test_ll_ast_builder_examples() {
    ASSERT_EQUAL((string) "{\"PROG\":[\"T_NUMBER(123)\",\"T_EOP()\"]}",
                 ast_node_to_json(build_ll_ast("PROG: T_NUMBER T_EOP", "123")));
  }

  void test_ll_ast_builder_one_lookahead() {
    ASSERT_EQUAL(
      (string) "{\"PROG\":[{\"A\":[{\"B\":[\"T_SUB(-)\"]},\"T_NUMBER(12)\"]}]}",
      ast_node_to_json(build_ll_ast(
        "PROG: A\nA: B T_NUMBER | B T_STRING\nB: T_ADD | T_SUB",
        "-12",
        2
      ))
    );
  }

  // PRIVATES ///////////////////////////////////////////////////////////////

private:
  bool assert(const bool test, const char *caller_name, unsigned int line_no) {
    if (test) {
      success_count++;
      cout << "\x1B[92mPASS\x1B[0m " << caller_name << " @" << line_no << endl;
    } else {
      failure_count++;
      cout << "\x1B[91mFAIL\x1B[0m " << caller_name << " @" << line_no << endl;
    }
    return test;
  };

  template <typename T>
  bool assert_eq(const T lhs, const T rhs, const char *caller_name,
                 unsigned int line_no) {
    bool result = assert(lhs == rhs, caller_name, line_no);

    if (!result) {
      cout << "  \x1B[94mEXPECTED: " << lhs << "\x1B[0m\n";
      cout << "    \x1B[95mACTUAL: " << rhs << "\x1B[0m\n";
    }

    return result;
  }

  void test_tokenizer_recognize_type(TokenType type,
                                     vector<string> test_cases) {
    for (const auto &test_case : test_cases) {
      Tokenizer t(new_tokenizer_from_string(test_case));
      ASSERT_EQUAL((size_t)2, t.tokens.size());
      ASSERT_EQUAL(type, t.tokens[0].type);
      ASSERT_EQUAL(test_case, t.tokens[0].value);
    }
  }

  AstNode *build_ast(string raw_grammar, string raw_source) {
    istringstream grammar_iss(raw_grammar);
    istream_iterator<string> grammar_isit(grammar_iss);
    Grammar *pg = new Grammar(grammar_isit);

    istringstream source_iss(raw_source);
    source_iss >> noskipws;
    istream_iterator<char> source_isit(source_iss);
    Tokenizer *pt = new Tokenizer(source_isit);

    AstBuilder a(pg, pt);
    return a.build();
  }

  AstNode *build_ll_ast(string raw_grammar, string raw_source, unsigned int ll_max_level = 1) {
    istringstream grammar_iss(raw_grammar);
    istream_iterator<string> grammar_isit(grammar_iss);
    Grammar *pg = new Grammar(grammar_isit);

    istringstream source_iss(raw_source);
    source_iss >> noskipws;
    istream_iterator<char> source_isit(source_iss);
    Tokenizer *pt = new Tokenizer(source_isit);

    LLAstBuilder a(pg, pt, ll_max_level);
    return a.build();
  }

  string ast_node_to_json(const AstNode *node) {
    ostringstream oss;
    oss << *node;
    return oss.str();
  }

  void test_grammar_normalizer_expect(string original, string expected) {
    Grammar g(new_grammar_from_string(original));

    GrammarNormalizer gn(&g);
    gn.normalize();
    ostringstream normalized;
    normalized << g;

    ASSERT_EQUAL(expected, normalized.str());
  }
};
