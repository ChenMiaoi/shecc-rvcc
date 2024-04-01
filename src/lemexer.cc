#include "lexemer.h"

auto lexemer::__load_source(const std::string& source_file) -> void {
  std::string source_line;

  fs::path file_path = fs::canonical(source_file);

  if (!fs::exists(file_path)) {
    spdlog::error("\"{}\" file not exists!", source_file);
    abort();
  }

  std::ifstream file(source_file);

  if (!file.is_open()) {
    spdlog::error("\"{}\" file cannot open!", source_file);
    abort();
  }
  spdlog::info("open \"{}\" file.", source_file);

  while (std::getline(file, source_line)) {
    if (!source_line.compare(0, 9, "#include ") && source_line[9] == '"') {
      spdlog::info("matched include.");

      std::string path = file_path.parent_path();
      spdlog::info("get the file directory: {}", path);

      path += '/';
      path += source_line.substr(
        source_line.find('"') + 1,
        source_line.rfind('"') - source_line.find('"') - 1
      );
      spdlog::info("get the source file include header path: {}", path);

      __load_source(path);
    } else {
      _source += source_line;
      _source += '\n';
    }
  }

  spdlog::info("source is: \n{}\nsource idx is {}", _source, _source.size());
  file.close();
}

lexemer::lexemer(const std::string &path)
  : _current {0}
  , _start {0}
  , _line {1}
  , _source {} {
  __load_source(path);
}

auto lexemer::scan_tokens() -> void {
  expect(token_t::t_start);
}

auto lexemer::expect(token_t token, int32_t aliasing) -> void {
  if (_next_token != token)
    error("Unexpected token");
  spdlog::info("expect token type: {}", me::enum_name(token));

  _next_token = get_next_token(aliasing);
  spdlog::info("get next token: {}", me::enum_name(_next_token));
}

auto lexemer::get_next_token(int32_t aliasing) -> token_t {
  std::string token_str;

  if (_source[_current] == '#') {
    do {
      token_str.push_back(_source[_current]);
    } while (is_alnum(read_char(true)));
    skip_whitespace();
  }
}

#if 0
auto lexemer::is_digit(char c) -> bool {
  return c >= '0' && c <= '9';
}

auto lexemer::is_hex(char c) -> bool {
  return is_digit(c) ||
         c == 'x'    ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

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
  spdlog::info("get the next token: {}", me::enum_name(next_token));
}

auto lexemer::token_internal(int32_t aliasing) -> token_t {
  // ÅÐ¶ÏÔ¤´¦Àí¹Ø¼ü×Ö
  if (next_char == '#') {
    do {
      token_str.push_back(next_char);
    } while ( is_alnum(read_char(false)) );
    skip_whitespace();

    spdlog::info("token str is {}", token_str);

    if (token_str == "#include") return token_t::t_cppd_include;
    if (token_str == "#define") return token_t::t_cppd_define;
    if (token_str == "#undef") return token_t::t_cppd_undef;
    if (token_str == "#error") return token_t::t_cppd_error;
    if (token_str == "#if") return token_t::t_cppd_if;
    if (token_str == "#elif") return token_t::t_cppd_elif;
    if (token_str == "#ifdef") return token_t::t_cppd_ifdef;
    if (token_str == "#else") return token_t::t_cppd_else;
    if (token_str == "#endif") return token_t::t_cppd_endif;

    error("Unknown directive");
  }
  // ÅÐ¶ÏC·ç¸ñ×¢ÊÍ
  if (next_char == '/') {
    read_char(false);
    if (next_char == '*') {
      do {
        read_char(false);
        if (next_char == '*') {
          read_char(false);
          if (next_char == '/') {
            read_char(true);
            return token_internal(aliasing);
          }
        }
      } while (next_char);
    } else {
      // Èç¹ûÊÇµ¥¸ö '/' ÄÇÃ´¾Í±íÊ¾ÊÇ³ý·¨
      if (next_char == ' ')
        read_char(false);
      return token_t::t_divide;
    }
    error("Unexpected '/'");
  }
  // ÅÐ¶ÏÊý×Ö
  if (is_digit(next_char)) {
    token_str.clear();
    do {
      token_str.push_back(next_char);
    } while (is_hex(read_char(false)));
    skip_whitespace();
    return token_t::t_numeric;
  }
  // ÅÐ¶Ï×óÀ¨ºÅ
  if (next_char == '(') {
    read_char(true);
    return token_t::t_open_bracket;
  }
  // ÅÐ¶ÏÓÒÀ¨ºÅ
  if (next_char == ')') {
    read_char(true);
    return token_t::t_close_bracket;
  }
  // ÅÐ¶Ï×ó»¨À¨ºÅ
  if (next_char == '{') {
    read_char(true);
    return token_t::t_open_curly;
  }
  // ÅÐ¶ÏÓÒ»¨À¨ºÅ
  if (next_char == '}') {
    read_char(true);
    return token_t::t_open_curly;
  }
  // ÅÐ¶Ï×óÖÐÀ¨ºÅ
  if (next_char == '[') {
    read_char(true);
    return token_t::t_open_square;
  }
  // ÅÐ¶ÏÓÒÖÐÀ¨ºÅ
  if (next_char == ']') {
    read_char(true);
    return token_t::t_close_square;
  }
  // ÅÐ¶ÏÒì»ò
  if (next_char == '^') {
    read_char(true);
    return token_t::t_bit_xor;
  }
  // ÅÐ¶ÏÈ¡·´
  if (next_char == '~') {
    read_char(true);
    return token_t::t_bit_not;
  }
  // ÅÐ¶Ï×Ö·û´®
  if (next_char == '"') {
    token_str.clear();
    bool special = false;

    while ((read_char(false) != '"') || special) {
      // ÅÐ¶ÏÊÇ·ñÎª×ªÒå×Ö·û
      if (token_str.back() == '\\') {
        if (next_char == 'n') token_str.back() = '\n';
        else if (next_char == '"') token_str.back() = '"';
        else if (next_char == 'r') token_str.back() = '\r';
        else if (next_char == '\'') token_str.back() = '\'';
        else if (next_char == 't') token_str.back() = '\t';
        else if (next_char == '\\') token_str.back() = '\\';
        else abort();
      } else {
        token_str.push_back(next_char);
      }
      // Èç¹ûÓöµ½'\\'¾ÍÓ¦¸Ã¿¼ÂÇÊÇ·ñÎª×ªÒå
      if (next_char == '\\') special = true;
      else special = false;
    }
    read_char(true);
    return token_t::t_string;
  }
  // ÅÐ¶Ï×Ö·û
  if (next_char == '\'') {
    read_char(false);
    token_str.clear();

    if (next_char == '\\') {
      read_char(false);
      if (next_char == 'n') token_str.back() = '\n';
      else if (next_char == '"') token_str.back() = '"';
      else if (next_char == 'r') token_str.back() = '\r';
      else if (next_char == '\'') token_str.back() = '\'';
      else if (next_char == 't') token_str.back() = '\t';
      else if (next_char == '\\') token_str.back() = '\\';
      else abort();
    } else {
      token_str.push_back(next_char);
    }
    if (read_char(false) != '\'') abort();
    read_char(true);
    return token_t::t_char;
  }

  return token_t::t_eof;
}

auto lexemer::accept(token_t token) -> bool {
  spdlog::info("accept token: {}", me::enum_name(token));
  return accept_internal(token, 1);
}

auto lexemer::accept_internal(token_t token, int32_t aliasing) -> bool {
  if (next_token == token) {
    next_token = token_internal(aliasing);
    spdlog::info("accept get next token: {}", me::enum_name(token));
    return true;
  }
  return false;
}

auto lexemer::ident(token_t token, std::string &value) -> void {
  ident_internal(token, value, 1);
}

auto lexemer::ident_internal(token_t token, std::string &value, int32_t aliasing) -> void {
  if (next_token != token)
    error("Unexpected token");
  value = token_str;
  next_token = token_internal(aliasing);

  spdlog::info("ident token: {}\nvalue: {}\nnext token: {}",
               me::enum_name(token), value, me::enum_name(next_token));
}

auto lexemer::peek_token(token_t token, const std::string &value) -> bool {
  if (next_token == token) {
    if (!value.empty()) return true;
    token_str = value;
    return true;
  }
  return false;
}

auto lexemer::read_global_statement() -> void {
  std::string token;
  block_t* block = &gs.block_source[0];

  if (accept(token_t::t_struct)) {
    ident(token_t::t_identifier, token);
  } else if (accept(token_t::t_typedef)) {

  } else if (peek_token(token_t::t_identifier, "")) {

  } else {
    error("Syntax error in global statement");
  }
}

auto lexemer::read_preproc_directive() -> bool {
  std::string token;

  if (peek_token(token_t::t_cppd_include, token)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_include));

    expect(token_t::t_cppd_include);

    if (peek_token(token_t::t_string, "")) {
      expect(token_t::t_string);
    }
    return true;
  }

  if (accept(token_t::t_cppd_define)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_define));
    return true;
  }

  if (peek_token(token_t::t_cppd_undef, token)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_undef));
    return true;
  }

  if (peek_token(token_t::t_cppd_error, token)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_error));
    return true;
  }

  if (accept(token_t::t_cppd_if)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_if));
    return true;
  }

  if (accept(token_t::t_cppd_elif)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_elif));
    return true;
  }

  if (accept(token_t::t_cppd_else)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_else));
    return true;
  }

  if (accept(token_t::t_cppd_endif)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_endif));
    return true;
  }

  if (accept_internal(token_t::t_cppd_ifdef, 0)) {
    spdlog::info("entry token: {}", me::enum_name(token_t::t_cppd_ifdef));
    return true;
  }

  return false;
}
#endif
