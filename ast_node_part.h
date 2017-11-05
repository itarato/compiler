#pragma once

#include <iostream>

#include "token.h"

class AstNode;
ostream & operator<<(ostream &, const AstNode &);

using namespace std;

class AstNodePart {
public:
  AstNode *p_node;
  Token token;
  bool is_node;

  AstNodePart(bool);
  ~AstNodePart();

  friend ostream & operator<<(ostream & os, const AstNodePart & self) {
    if (self.is_node) {
      os << *self.p_node << endl;
    } else {
      os << self.token << endl;
    }

    return os;
  };
};

AstNodePart* new_ast_node_part_node(AstNode *);
AstNodePart* new_ast_node_part_token(Token);
