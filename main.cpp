#include <iostream>
#include <cstring>

#include "grammar.h"
#include "tokenizer.h"
#include "ast_builder.h"
#include "ast_node.h"

using namespace std;

int mode_parse(int, char *[]);
int mode_grammar_translate(int, char *[]);
void print_help();

// Receives:
//  - grammar file name
//  - source file name
int main(int argc, char *argv[]) {
  if (argc <= 1) {
      print_help();
      exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "parse") == 0) {
    exit(mode_parse(argc, argv));
  } else if (strcmp(argv[1], "grammar") == 0) {
    exit(mode_grammar_translate(argc, argv));
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
  AstNode * p_ast_node = ab.build();

  if (p_ast_node != nullptr)
    cout << *p_ast_node << endl;

  return EXIT_SUCCESS;
}

int mode_grammar_translate(int argc, char *argv[]) {
  if (argc != 3) {
    print_help();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void print_help() {
  cout << "Invocation error.\n";
  cout << "Usage for parsing: ./main parse GRAMMAR SOURCE\n";
  cout << "Usage for grammar correction: ./main grammar GRAMMAR\n";
}
