#include <iostream>

#include "grammar.h"
#include "tokenizer.h"
#include "ast_builder.h"
#include "ast_node.h"

using namespace std;

int main() {
  Grammar g;
  cout << g << endl;

  Tokenizer t;
  cout << t << endl;

  AstBuilder ab(&g, &t);
  AstNode * p_ast_node = ab.build();

  if (p_ast_node != nullptr)
    cout << *p_ast_node << endl;

  exit(EXIT_SUCCESS);
}
