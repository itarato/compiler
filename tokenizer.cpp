#include <ctype.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <fstream>

#include "tokenizer.h"
#include "util.h"

using namespace std;

Tokenizer::Tokenizer(istream_iterator<char> _source_if) : source_if(_source_if) {
    for (;;) {
        for (; is_whitespace(*source_if) && !is_end(); source_if++) {
        }

        if (is_end()) break;

        tokens.push_back(get_token());
    }

    tokens.push_back({TokenType::EOP, ""});
}

Token Tokenizer::get_token() {
    char ch = *source_if;
    if (is_numeric(ch)) {
        return read_number_token();
    } else if (is_quote(ch)) {
        return read_string_token();
    } else if (is_op(ch)) {
        source_if++;
        return read_op_token(ch);
    } else if (is_eq(ch)) {
        source_if++;
        return Token(TokenType::ASSIGN, string{ch});
    } else if (ch == ',') {
        source_if++;
        return Token(TokenType::COMMA, string{ch});
    } else if (ch == ';') {
        source_if++;
        return Token(TokenType::SEMICOLON, string{ch});
    } else if (ch == '{') {
        source_if++;
        return Token(TokenType::BRACE_OPEN, string{ch});
    } else if (ch == '}') {
        source_if++;
        return Token(TokenType::BRACE_CLOSE, string{ch});
    } else if (ch == '(') {
        source_if++;
        return Token(TokenType::PAREN_OPEN, string{ch});
    } else if (ch == ')') {
        source_if++;
        return Token(TokenType::PAREN_CLOSE, string{ch});
    } else if (is_letter(ch)) {
        string s = read_char_list_token();
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

bool Tokenizer::is_end() {
    return source_if == istream_iterator<char>();
}

bool Tokenizer::is_keyword(string s) {
    return keyword_translation_map.find(s) != keyword_translation_map.end();
}

Token Tokenizer::wrap_keyword(string s) {
    return Token(keyword_translation_map[s], s);
}

Token Tokenizer::read_number_token() {
    string value;
    while (is_numeric(*source_if) && !is_end()) {
        value.push_back(*(source_if++));
    }
    return Token(TokenType::NUMBER, value);
}

Token Tokenizer::read_string_token() {
    char quote = *(source_if++);

    string value;

    while (*source_if != quote && !is_end()) value.push_back(*(source_if++));

    if (*source_if != quote) {
        cout << "Broken string value token - missing closing quote pair.\n";
        exit(EXIT_FAILURE);
    }
    source_if++;

    return Token(TokenType::STRING, value);
}

string Tokenizer::read_char_list_token() {
    string value;
    while ((is_letter(*source_if) || is_numeric(*source_if)) && !is_end()) {
        value.push_back(*(source_if++));
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

bool Tokenizer::is_eq(char ch) { return ch == '='; }

Tokenizer new_tokenizer_from_filename(char *filename) {
    ifstream ifs(filename);
    ifs >> noskipws;
    istream_iterator<char> isit(ifs);
    return Tokenizer(isit);
}

Tokenizer new_tokenizer_from_string(string raw) {
    istringstream iss(raw);
    iss >> noskipws;
    istream_iterator<char> isit(iss);
    return Tokenizer(isit);
}

