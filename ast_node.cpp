#include <utility>

#include "ast_node.h"

using namespace std;

AstNode::AstNode(string rule) : rule(rule) {}

AstNode::~AstNode() {
  for (auto part : parts) {
    delete part;
  }
}
