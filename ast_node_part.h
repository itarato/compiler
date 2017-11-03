#pragma once

#include "token.h"

class AstNode;

using namespace std;

class AstNodePart {
public:
  union {
    AstNode *p_node;
    Token token;
  };
  bool is_node;

  AstNodePart(bool);
  ~AstNodePart();

  AstNodePart(AstNodePart &&) = default;
  AstNodePart(const AstNodePart &) = delete;
};
AstNodePart&& new_ast_node_part_node(AstNode *);
AstNodePart&& new_ast_node_part_token(Token);
