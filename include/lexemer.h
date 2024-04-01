#ifndef __SHECC_RVCC_LEXEMER_H__
#define __SHECC_RVCC_LEXEMER_H__

#include "defs.h"
#include "global.h"
#include "token.h"

#include "spdlog/spdlog.h"
#include "magic_enum/magic_enum.hpp"

#include <filesystem>
#include <fstream>
#include <cstdint>
#include <string>

namespace fs = std::filesystem;
namespace me = magic_enum;

class lexemer {
private:
  int32_t _current;
  int32_t _start;
  int32_t _line;
  std::string _source;
  std::string _literal;
  token_t     _next_token;
  std::vector<token> _tokens;
//  std::unordered_map<std::string, token_t> _key_words;

public:
  lexemer(const std::string& path);

  auto scan_tokens() -> void;
  auto expect(token_t token, int32_t aliasing = 1) -> void;
  auto get_next_token(int32_t aliasing) -> token_t;

  auto read_char(bool is_skip_space) -> char;
  auto is_alnum(char c) -> bool;
  auto skip_whitespace() -> void;
private:
  auto __load_source(const std::string& source_file) -> void;

};

#if 0
struct lexemer: __singleton<lexemer> {
  friend __singleton<lexemer>;

  std::string token_str;
  token_t next_token;
  int8_t next_char;
  int32_t skip_newline = 1;

public:
  auto expect(token_t token) -> void;
  auto expect_internal(token_t token, int32_t aliasing) -> void;
  auto token_internal(int32_t aliasing) -> token_t;
  auto accept(token_t token) -> bool;
  auto accept_internal(token_t token, int32_t aliasing) -> bool;
  auto ident(token_t token, std::string& value) -> void;
  auto ident_internal(token_t token, std::string& value, int32_t aliasing) -> void;
  auto peek_token(token_t token, const std::string& value) -> bool;

public:
  auto is_digit(char c) -> bool;
  auto is_hex(char c) -> bool;
  auto is_alnum(char c) -> bool;
  auto read_char(bool is_skip_space) -> int8_t;
  auto peek_char(int32_t offset) -> int8_t;

  auto is_newline(char c) -> bool;
  auto is_linebreak(char c) -> bool;
  auto is_whitespace(char c) -> bool;
  auto skip_whitespace() -> void ;

  auto read_preproc_directive() -> bool;
  auto read_global_statement() -> void;

private:
  lexemer() = default;
};

#endif

#endif //! __SHECC_RVCC_LEXEMER_H__
