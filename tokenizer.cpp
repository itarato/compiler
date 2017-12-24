#include <ctype.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "tokenizer.h"
#include "util.h"

using namespace std;

Tokenizer::Tokenizer(char *source_file_name) {
    ifstream source_if(source_file_name);

    for (;;) {
        for (; is_whitespace(source_if.peek()); source_if.get()) {
        }

        if (source_if.peek() == EOF) break;

        tokens.push_back(get_token(&source_if));
    }

    tokens.push_back({TokenType::EOP, ""});
}

Token Tokenizer::get_token(ifstream *source_if) {
    char ch = source_if->peek();
    if (is_numeric(ch)) {
        return read_number_token(source_if);
    } else if (is_quote(ch)) {
        return read_string_token(source_if);
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
    } else if (ch == '(') {
        source_if->get();
        return Token(TokenType::PAREN_OPEN, string{ch});
    } else if (ch == ')') {
        source_if->get();
        return Token(TokenType::PAREN_CLOSE, string{ch});
    } else if (is_letter(ch)) {
        string s = read_char_list_token(source_if);
        if (is_keyword(s)) {
            return wrap_keyword(s);
        } else {
            return Token(TokenType::NAME, s);
        }
    } else {
        cout << "Unexpected char: " << (int)ch << endl;
        exit(EXIT_FAILURE);
    }
}

bool Tokenizer::is_keyword(string s) {
    return keyword_translation_map.find(s) != keyword_translation_map.end();
}

Token Tokenizer::wrap_keyword(string s) {
    return Token(keyword_translation_map[s], s);
}

Token Tokenizer::read_number_token(ifstream *source_if) {
    string value;
    while (is_numeric(source_if->peek())) {
        value.push_back(source_if->get());
    }
    return Token(TokenType::NUMBER, value);
}

Token Tokenizer::read_string_token(ifstream *source_if) {
    char quote = source_if->get();

    string value;

    while (source_if->peek() != quote) value.push_back(source_if->get());
    source_if->get();

    return Token(TokenType::STRING, value);
}

string Tokenizer::read_char_list_token(ifstream *source_if) {
    string value;
    while (is_letter(source_if->peek())) {
        value.push_back(source_if->get());
    }
    return value;
}

Token Tokenizer::read_op_token(char ch) {
    switch (ch) {
        case '+':
            return Token(TokenType::OP_ADD, string{ch});
        case '-':
            return Token(TokenType::OP_SUB, string{ch});
        case '*':
            return Token(TokenType::OP_MUL, string{ch});
        case '/':
            return Token(TokenType::OP_DIV, string{ch});
        case '<':
            return Token(TokenType::OP_LT, string{ch});
        case '>':
            return Token(TokenType::OP_GT, string{ch});
        default:
            cout << "Unknown OP: " << ch << endl;
            exit(EXIT_FAILURE);
    }
}

bool Tokenizer::is_numeric(char ch) { return ch >= '0' && ch <= '9'; }

bool Tokenizer::is_letter(char ch) { return ch >= 'a' && ch <= 'z'; }

bool Tokenizer::is_op(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' ||
           ch == '>';
}

bool Tokenizer::is_whitespace(char ch) { return isspace(ch); }

bool Tokenizer::is_quote(char ch) { return ch == '"' || ch == '\''; }
