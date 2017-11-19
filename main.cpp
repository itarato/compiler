#include <iostream>

#include "grammar.h"
#include "tokenizer.h"
#include "ast_builder.h"
#include "ast_node.h"

using namespace std;

// Receives:
//  - grammar file name
//  - source file name
int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Invocation error 2 parameters are needed, found " << argc - 1 << "." << endl;
    cout << "Usage: ./main GRAMMAR SOURCE\n";
    exit(EXIT_FAILURE);
  }

  Grammar g(argv[1]);
  cout << g << endl;

  Tokenizer t(argv[2]);
  cout << t << endl;

  AstBuilder ab(&g, &t);
  AstNode * p_ast_node = ab.build();

  if (p_ast_node != nullptr)
    cout << *p_ast_node << endl;

  exit(EXIT_SUCCESS);
}
