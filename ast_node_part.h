#pragma once

#include <utility>

#include "token.h"

class AstNode;

using namespace std;

struct AstNodePart {
  union {
    AstNode *p_node;
    Token token;
  };
  bool is_node;

  AstNodePart(bool);
  ~AstNodePart();
};
AstNodePart&& new_ast_node_part_node(AstNode *);
AstNodePart&& new_ast_node_part_token(Token);
