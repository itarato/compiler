#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "grammar.h"
#include "tokenizer.h"

using namespace std;

#define ASSERT(t) assert((t), __FUNCTION__, __LINE__)
#define ASSERT_EQUAL(lhs, rhs) assert_eq(lhs, rhs, __FUNCTION__, __LINE__)

class Test {
   public:
    unsigned int success_count;
    unsigned int failure_count;
    vector<string> errors;

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

        cout << endl
             << "\x1B[1mCOMPLETE\x1B[0m - \x1B[92mSUCCESS: " << success_count
             << "\x1B[0m \x1B[91mFAILURE: " << failure_count << "\x1B[0m"
             << endl
             << endl;

        if (errors.size() > 0) {
            cout << "\x1B[91mERRORS:\n";
            copy(errors.begin(), errors.end(),
                 ostream_iterator<string>(cout, "\n"));
            cout << "\x1B[0m";
        }
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
        vector<string> test_cases{"puts", "puts ", "puts\n",
                                  "puts \t \n \n \r"};

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
    }

   private:
    void assert(const bool test, const char *caller_name,
                unsigned int line_no) {
        if (test) {
            success_count++;
            cout << "\x1B[94m.\x1B[0m";
        } else {
            string error_message(caller_name);
            error_message += " at line ";
            error_message += to_string(line_no);
            errors.push_back(error_message);
            failure_count++;
            cout << "\x1B[91mF\x1B[0m";
        }
    };

    template <typename T>
    void assert_eq(const T lhs, const T rhs, const char *caller_name,
                   unsigned int line_no) {
        assert(lhs == rhs, caller_name, line_no);
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
};
