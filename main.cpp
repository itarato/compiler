#include <iostream>

#include "grammar.h"
#include "tokenizer.h"
#include "ast_builder.h"

using namespace std;

int main() {
  Grammar g;
  cout << g << endl;

  Tokenizer t;
  cout << t << endl;

  AstBuilder ab(&g, &t);
  ab.build();

  exit(EXIT_SUCCESS);
}
