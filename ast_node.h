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
    os << "{" << endl;
    os << self.rule << ": [" << endl;

    for (auto & part : self.parts) {
      os << *part << "," << endl;
    }

    os << "]";
    os << "}" << endl;

    return os;
  };
};
