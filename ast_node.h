#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "ast_node_part.h"

using namespace std;

struct AstNode {
  string rule;
  vector<AstNodePart *> parts;

  AstNode(string);
  ~AstNode();

  friend ostream &operator<<(ostream &os, const AstNode &self) {
    os << "{\"" << self.rule << "\":[";

    for (auto it = self.parts.begin(); it != self.parts.end(); it++) {
      os << **it;
      if ((it + 1) != self.parts.end()) os << ",";
    }

    os << "]}";

    return os;
  };
};
