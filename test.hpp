#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "grammar.h"

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

        cout << endl
             << "COMPLETE - SUCCESS: " << success_count
             << " FAILURE : " << failure_count << endl
             << endl;

        if (errors.size() > 0) {
            cout << "ERRORS:\n";
            copy(errors.begin(), errors.end(),
                 ostream_iterator<string>(cout, "\n"));
        }
    };

    void test_grammar_basic() {
        Grammar g(make_grammar("PROG: T_EOF"));

        ASSERT_EQUAL((size_t)1, g.lines.size());
        ASSERT(g.lines.find("PROG") != g.lines.end());
        ASSERT_EQUAL((size_t)1, g.lines["PROG"].rules.size());
        ASSERT_EQUAL((size_t)1, g.lines["PROG"].rules[0].parts.size());
        ASSERT_EQUAL((string)"T_EOF", g.lines["PROG"].rules[0].parts[0]);
    };

    void test_grammar_multiline() {
        Grammar g(make_grammar("A: B\nB: T_C"));

        ASSERT_EQUAL((size_t)2, g.lines.size());
    }

    void test_grammar_multiple_rules() {
        Grammar g(make_grammar("A: B | C | D"));

        ASSERT_EQUAL((size_t)3, g.lines["A"].rules.size());
    }

    void test_grammar_empty_rule() {
        Grammar g1(make_grammar("A: "));
        ASSERT_EQUAL((size_t)0, g1.lines["A"].rules[0].parts.size());

        Grammar g2(make_grammar("A: B |"));
        ASSERT_EQUAL((size_t)0, g2.lines["A"].rules[1].parts.size());

        Grammar g3(make_grammar("A: B | | C"));
        ASSERT_EQUAL((size_t)0, g3.lines["A"].rules[1].parts.size());
    }

   private:
    Grammar make_grammar(string s) {
        istringstream iss(s);
        istream_iterator<string> isi(iss);
        return Grammar(isi);
    }

    void assert(const bool test, const char *caller_name,
                unsigned int line_no) {
        if (test) {
            success_count++;
            cout << '.';
        } else {
            string error_message(caller_name);
            error_message += " at line ";
            error_message += to_string(line_no);
            errors.push_back(error_message);
            failure_count++;
            cout << 'F';
        }
    };

    template <typename T>
    void assert_eq(const T lhs, const T rhs, const char *caller_name,
                   unsigned int line_no) {
        assert(lhs == rhs, caller_name, line_no);
    }
};
