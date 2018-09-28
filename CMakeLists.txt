cmake_minimum_required (VERSION 3.0)
project (Compiler)
add_executable(compiler ast_builder.cpp  ast_node.cpp  ast_node_part.cpp  grammar.cpp  grammar_line.cpp  grammar_normalizer.cpp  grammar_rule.cpp  ll_ast_builder.cpp  logger.cpp  lookahead_table_generator.cpp  main.cpp  token.cpp  tokenizer.cpp  util.cpp
 test.hpp)

set(CMAKE_CXX_STANDARD 17)
