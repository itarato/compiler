#include <cstring>
#include <iostream>

#include "ast_builder.h"
#include "ast_node.h"
#include "grammar.h"
#include "grammar_normalizer.h"
#include "ll_ast_builder.h"
#include "test.hpp"
#include "tokenizer.h"

using namespace std;

int mode_parse(int, char *[]);
int mode_llparse(int, char *[]);
int mode_grammar_translate(int, char *[]);
int mode_test();
void print_help();

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        print_help();
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "parse") == 0) {
        exit(mode_parse(argc, argv));
    } else if (strcmp(argv[1], "grammar") == 0) {
        exit(mode_grammar_translate(argc, argv));
    } else if (strcmp(argv[1], "llparse") == 0) {
        exit(mode_llparse(argc, argv));
    } else if (strcmp(argv[1], "test") == 0) {
        exit(mode_test());
    }

    exit(EXIT_FAILURE);
}

int mode_parse(int argc, char *argv[]) {
    if (argc != 4) {
        print_help();
        return EXIT_FAILURE;
    }

    Grammar g(argv[2]);
    cout << g << endl;

    Tokenizer t(argv[3]);
    cout << t << endl;

    AstBuilder ab(&g, &t);
    AstNode *p_ast_node = ab.build();

    if (p_ast_node != nullptr) cout << *p_ast_node << endl;

    return EXIT_SUCCESS;
}

int mode_llparse(int argc, char *argv[]) {
    if (argc != 4) {
        print_help();
        return EXIT_FAILURE;
    }

    Grammar g(argv[2]);
    cout << g << endl;

    Tokenizer t(argv[3]);
    cout << t << endl;

    LLAstBuilder llab(&g, &t);
    llab.build();

    return EXIT_SUCCESS;
}

int mode_grammar_translate(int argc, char *argv[]) {
    if (argc != 3) {
        print_help();
        return EXIT_FAILURE;
    }

    Grammar g(argv[2]);

    GrammarNormalizer gn(&g);
    gn.normalize();

    cout << g << endl;

    return EXIT_SUCCESS;
}

int mode_test() {
    Test t;
    t.run();

    return EXIT_SUCCESS;
}

void print_help() {
    cout << "Invocation error.\n";
    cout << "Usage for parsing: ./main parse GRAMMAR SOURCE\n";
    cout << "Usage for grammar correction: ./main grammar GRAMMAR\n";
}
