#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ast_builder.h"
#include "ast_node.h"
#include "bottom_top_ast_builder.h"
#include "globals.h"
#include "grammar.h"
#include "grammar_normalizer.h"
#include "ll_ast_builder.h"
#include "logger.h"
#include "test.hpp"
#include "tokenizer.h"

using namespace std;

void sigsegv_handler(int);
int mode_parse(int, char *[]);
int mode_llparse(int, char *[]);
int mode_btparse(int, char *[]);
int mode_grammar_translate(int, char *[]);
int mode_test();
void print_help() noexcept;

int main(int argc, char *argv[]) {
  signal(SIGSEGV, sigsegv_handler);

  logger.info("Bin boot");

  if (argc <= 1) {
    print_help();
    exit(EXIT_FAILURE);
  }

  int res;

  if (strcmp(argv[1], "parse") == 0) {
    res = mode_parse(argc, argv);
  } else if (strcmp(argv[1], "grammar") == 0) {
    res = mode_grammar_translate(argc, argv);
  } else if (strcmp(argv[1], "llparse") == 0) {
    res = mode_llparse(argc, argv);
  } else if (strcmp(argv[1], "btparse") == 0) {
    res = mode_btparse(argc, argv);
  } else if (strcmp(argv[1], "test") == 0) {
    res = mode_test();
  } else {
    exit(EXIT_FAILURE);
  }

  logger.info("Finish");

  exit(res);
}

int mode_parse(int argc, char *argv[]) {
  if (argc != 4) {
    print_help();
    return EXIT_FAILURE;
  }

  Grammar g(new_grammar_from_filename(argv[2]));
  cout << g << endl;

  Tokenizer t(new_tokenizer_from_filename(argv[3]));
  cout << t << endl;

  AstBuilder ab(&g, &t, true);
  unique_ptr<AstNode> p_ast_node(ab.build());

  if (p_ast_node != nullptr) cout << *p_ast_node << endl;

  return EXIT_SUCCESS;
}

int mode_llparse(int argc, char *argv[]) {
  if (argc != 5) {
    print_help();
    return EXIT_FAILURE;
  }

  Grammar g(new_grammar_from_filename(argv[2]));
  cout << g << endl;

  Tokenizer t(new_tokenizer_from_filename(argv[3]));
  cout << t << endl;

  unsigned int ll_level = (unsigned int)stoi(argv[4]);
  cout << "Lookahead level: " << ll_level << endl;

  LLAstBuilder llab(&g, &t, ll_level, true);
  unique_ptr<AstNode> p_ast_node(llab.build());

  if (p_ast_node != nullptr)
    cout << *p_ast_node << endl;
  else
    cout << "Source is not parsable.\n";

  return EXIT_SUCCESS;
}

int mode_btparse(int argc, char *argv[]) {
  if (argc != 4) {
    print_help();
    return EXIT_FAILURE;
  }

  Grammar g(new_grammar_from_filename(argv[2]));
  cout << g << endl;

  Tokenizer t(new_tokenizer_from_filename(argv[3]));
  cout << t << endl;

  BottomTopAstBuilder btab(&g, &t, true);
  unique_ptr<AstNode> p_ast_node(btab.build());

  if (p_ast_node != nullptr)
    cout << *p_ast_node << endl;
  else
    cout << "Source is not parsable.\n";

  return EXIT_SUCCESS;
}

int mode_grammar_translate(int argc, char *argv[]) {
  if (argc != 3) {
    print_help();
    return EXIT_FAILURE;
  }

  ifstream ifs(argv[2]);
  istream_iterator<string> isi(ifs);
  Grammar g(isi);

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

void print_help() noexcept {
  cout << "Invocation error.\n";
  cout << "Usage for parsing: ./main parse GRAMMAR SOURCE\n";
  cout << "Usage for LL(N)-language parsing: ./main llparse GRAMMAR SOURCE "
          "LOOKAHEAD_LIMIT";
  cout << "Usage for bottom to top parsing: ./main btparse GRAMMAR SOURCE";
  cout << "Usage for grammar correction: ./main grammar GRAMMAR\n";
  cout << "Usage for testing: ./main test\n";
}

void sigsegv_handler(int sig) {
  void *array[20];
  int size = backtrace(array, 20);

  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
