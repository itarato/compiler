#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "ast_node_part.h"

using namespace std;

class AstNode {
public:
  string rule;
  vector<AstNodePart *> parts;

  AstNode(string);
  ~AstNode();

  friend ostream & operator<<(ostream & os, const AstNode & self) {
    os << "{\"" << self.rule << "\":[";

    for (auto it = self.parts.begin(); it != self.parts.end(); it++) {
      os << **it;
      if ((it + 1) != self.parts.end()) os << ",";
    }

    os << "]}";

    return os;
  };
};
