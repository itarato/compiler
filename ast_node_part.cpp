#include <utility>

#include "ast_node_part.h"
#include "ast_node.h"

AstNodePart::AstNodePart(bool is_node) : is_node(is_node) {}

AstNodePart::~AstNodePart() {
  if (is_node) {
    delete p_node;
  }
}

AstNodePart&& new_ast_node_part_node(AstNode *p_node) {
  AstNodePart anp(true);
  anp.p_node = p_node;
  return move(anp);
}

AstNodePart&& new_ast_node_part_token(Token token) {
  AstNodePart anp(false);
  anp.token = token;
  return move(anp);
}
