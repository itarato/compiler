#pragma once

#include <iostream>
#include <vector>
#include <iterator>
#include <string>

#include "token.h"

using namespace std;

class Tokenizer {
   public:
    istream_iterator<char> source_if;
    vector<Token> tokens;

    Tokenizer(istream_iterator<char>);

    friend ostream &operator<<(ostream &os, const Tokenizer &self) {
        os << "T[ ";
        for (const auto token : self.tokens) {
            os << token << " ";
        }
        os << "]";
        return os;
    };

   private:
    Token get_token();
    bool is_keyword(string);
    Token wrap_keyword(string);
    Token read_number_token();
    Token read_string_token();
    string read_char_list_token();
    Token read_op_token(char);
    bool is_numeric(char);
    bool is_letter(char);
    bool is_op(char);
    bool is_whitespace(char);
    bool is_quote(char);
    bool is_eq(char);
    bool is_end();
};

Tokenizer new_tokenizer_from_filename(char*);
Tokenizer new_tokenizer_from_string(string);

