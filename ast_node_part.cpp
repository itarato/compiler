#include <utility>
#include <iostream>

#include "token.h"
#include "ast_node_part.h"
#include "ast_node.h"

AstNodePart::AstNodePart(bool is_node) : p_node(nullptr), is_node(is_node) {}

AstNodePart::~AstNodePart() {
  if (is_node) {
    delete p_node;
  }
}

AstNodePart* new_ast_node_part_node(AstNode *p_node) {
  AstNodePart *anp = new AstNodePart(true);
  anp->p_node = p_node;
  return anp;
}

AstNodePart* new_ast_node_part_token(Token token) {
  AstNodePart *anp = new AstNodePart(false);
  anp->token = token;
  return anp;
}
