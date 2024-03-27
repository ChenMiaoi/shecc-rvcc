#include "lexemer.h"

auto lexemer::is_alnum(char c) -> bool {
  return isalpha(c) || isdigit(c) || c == '_';
}

auto lexemer::read_char(bool is_skip_space) -> int8_t {
  next_char = gs.source[++gs.source_idx];
  if (is_skip_space) skip_whitespace();
  return next_char;
}

auto lexemer::peek_char(int32_t offset) -> int8_t {
  return gs.source[gs.source_idx + offset];
}

auto lexemer::skip_whitespace() -> void {
  while ( true ) {
    if (is_linebreak(next_char)) {
      gs.source_idx += 2;
      next_char = gs.source[gs.source_idx];
      continue;
    }
    if (is_whitespace(next_char) ||
      (skip_newline && is_newline(next_char))) {
      next_char = gs.source[++gs.source_idx];
      continue;
    }
    break;
  }
}

auto lexemer::is_newline(char c) -> bool {
  return c == '\r' || c == '\n';
}

auto lexemer::is_linebreak(char c) -> bool {
  return c == '\\' && peek_char(1) == '\n';
}

auto lexemer::is_whitespace(char c) -> bool {
  return c == ' ' || c == '\t';
}

auto lexemer::expect(token_t token) -> void {
  expect_internal(token, 1);

  spdlog::info("expect token type: {}", me::enum_name(token));
}

auto lexemer::expect_internal(token_t token, int32_t aliasing) -> void {
  if (next_token != token) {
    error("Unexpected token");
  }
  next_token = token_internal(aliasing);
}

auto lexemer::token_internal(int32_t aliasing) -> token_t {
  if (next_char == '#') {
    int32_t i = 0;

    do {
      token_str.push_back(next_char);
    } while ( is_alnum(read_char(false)) );

    spdlog::info("token str is {}", token_str);
  }
  return token_t::t_eof;
}
