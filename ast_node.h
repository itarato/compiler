#pragma once

#include <vector>
#include <string>

#include "ast_node_part.h"

using namespace std;

class AstNode {
public:
  string rule;
  vector<AstNodePart> parts;

  AstNode(string);
};
