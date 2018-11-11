#include "bottom_top_ast_builder.h"
#include <algorithm>
#include <optional>
#include <sstream>
#include "globals.h"
#include "logger.h"
#include "util.h"

using namespace std;

struct BTState {
  size_t token_pos;
  int match_pos;
  optional<TokenList> substitution;
};

void debug_token_list(TokenList &tokens, StdLogger &logger) {
  ostringstream os;
  for (const auto &e : tokens) {
    if (holds_alternative<Token>(e)) {
      os << token_e_to_s(get<Token>(e).type);
    } else {
      os << get<string>(e);
    }
    os << ' ';
  }

  logger.info("Tokens: [ ", os.str(), ']');
}

BottomTopAstBuilder::BottomTopAstBuilder(Grammar *grammar, Tokenizer *tokenizer,
                                         bool verbose_mode)
    : rules(grammar->flatten()),
      tokenizer(tokenizer),
      verbose_mode(verbose_mode) {}

AstNode *BottomTopAstBuilder::build() {
  TokenList tokens;
  for_each(tokenizer->tokens.begin(), tokenizer->tokens.end(),
           [&tokens](Token t) { tokens.emplace_back(t); });
  vector<BTState> states{{0, 0, {}}};

  // Start from the last known good state.
  while (!states.empty()) {
    BTState state = states.back();
    states.pop_back();
    logger.info("State POP");

    logger.info("NEW CHECK -", state.token_pos, "-", state.match_pos);
    debug_token_list(tokens, logger);

    if (state.substitution.has_value()) {
      restore_substitute(&tokens, state.match_pos + 1,
                         state.substitution.value());
      logger.info("RESTORE AT", state.match_pos + 1);
      debug_token_list(tokens, logger);
    }

    if (state.token_pos >= tokens.size()) {
      logger.info("OVERFLOW");
      continue;
    }

    if (tokens.size() == 1) {
      logger.info("SUCCESS");
      return nullptr;
    }

    // Try finding a match for a rule.
    bool found_match = false;
    for (int match_pos = state.match_pos; match_pos >= 0; match_pos--) {
      optional<size_t> match = find_match(tokens, match_pos, state.token_pos);
      if (!match.has_value()) {
        continue;
      }

      found_match = true;

      logger.info("Has match with rule: ", rules[match.value()].second);
      TokenList erased =
          substitute(&tokens, match_pos, state.token_pos, match.value());

      states.push_back({(size_t)match_pos, match_pos - 1, {erased}});
      states.push_back({(size_t)(match_pos + 1), match_pos + 1, {}});
      states.push_back({(size_t)match_pos, match_pos, {}});

      break;
    }

    if (!found_match && !state.substitution.has_value()) {
      states.push_back({state.token_pos + 1, (int)state.token_pos + 1, {}});
      logger.info("NO MATCH -> SHIFT");
    }
  }

  return nullptr;
}

optional<size_t> BottomTopAstBuilder::find_match(TokenList &tokens,
                                                 size_t pos_lhs,
                                                 size_t pos_rhs) {
  size_t len = pos_rhs - pos_lhs + 1;
  for (int i = 0; i < rules.size(); i++) {
    if (rules[i].second.parts.size() != len) {
      continue;
    }

    bool has_match{true};
    for (size_t j = 0; j < len; j++) {
      string token;

      if (holds_alternative<Token>(tokens[pos_lhs + j])) {
        token = token_e_to_s(get<Token>(tokens[pos_lhs + j]).type);
      } else {
        token = get<string>(tokens[pos_lhs + j]);
      }
      if (rules[i].second.parts[j] != token) {
        has_match = false;
        break;
      }
    }

    if (has_match) {
      return {i};
    }
  }

  return {};
}

TokenList BottomTopAstBuilder::substitute(TokenList *tokens, size_t pos_lhs,
                                          size_t pos_rhs, size_t rule_pos) {
  TokenList erased{};

  size_t len = pos_rhs - pos_lhs + 1;
  for (int i = 0; i < len; i++) {
    erased.push_back(tokens->at(pos_lhs));
    tokens->erase(tokens->begin() + pos_lhs);
  }

  tokens->insert(tokens->begin() + pos_lhs,
                 variant<Token, string>(rules[rule_pos].first));

  return erased;
}

void BottomTopAstBuilder::restore_substitute(TokenList *tokens, size_t pos,
                                             TokenList substitution) {
  tokens->erase(tokens->begin() + pos);

  unsigned int offset{0};
  for (const auto &token : substitution) {
    tokens->insert(tokens->begin() + pos + offset++, token);
  }
}
