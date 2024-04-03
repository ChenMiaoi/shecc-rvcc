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

  while (std::getline(file, source_line, '\n')) {
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
  , _skip_newline {true}
  , _source {}
  , _tokens {{token_t::t_start, "start", "", 0}}{
  __load_source(path);
}

auto lexemer::scan_tokens() -> std::vector<token> {
  expect(token_t::t_start);
  do {
    _tokens.emplace_back(_next_token);
    expect(_next_token.token_type);
//    if (read_preproc_directive())
//      continue;
//    read_global_statement();
  } while (!accept(token_t::t_eof));
  _tokens.emplace_back(_next_token);

  return _tokens;
}

auto lexemer::expect(token_t token, int32_t aliasing) -> void {
  if (_next_token.token_type != token)
    error("Unexpected token");
  spdlog::info("#EXPECT# {}", _next_token);

  _next_token = get_next_token(aliasing);
  spdlog::info("#GET TOKEN# {}", _next_token);
}

auto lexemer::accept(token_t token, int32_t aliasing) -> bool {
  if (_next_token.token_type == token) {
    _next_token = get_next_token(aliasing);
    return true;
  }
  return false;
}

auto lexemer::peek(token_t token) -> bool {
  if (_next_token.token_type == token) {
    return true;
  }
  return false;
}

auto lexemer::get_next_token(int32_t aliasing) -> token {
  std::string token_str;

  if (peek_char() == '#') {
    do {
      token_str.push_back(peek_char());
    } while (is_alnum(read_char(false)));
    skip_whitespace();
    spdlog::info("get the token str: {}", token_str);

    if (token_str == "#include")  return {token_t::t_cppd_include, token_str, "", _line};
    if (token_str == "#define")   return {token_t::t_cppd_define, token_str, "", _line};
    if (token_str == "#undef")    return {token_t::t_cppd_undef, token_str, "", _line};
    if (token_str == "#error")    return {token_t::t_cppd_error, token_str, "", _line};
    if (token_str == "#if")       return {token_t::t_cppd_if, token_str, "", _line};
    if (token_str == "#elif")     return {token_t::t_cppd_elif, token_str, "", _line};
    if (token_str == "ifdef")     return {token_t::t_cppd_ifdef, token_str, "", _line};
    if (token_str == "#else")     return {token_t::t_cppd_else, token_str, "", _line};
    if (token_str == "endif")     return {token_t::t_cppd_endif, token_str, "", _line};
    error("Unknown directive");
  }

  // BUG or not BUG, we can get the token in comment?
  if (peek_char() == '/') {
    read_char();
    if (peek_char() == '*') {
      do {
        read_char();
        if (peek_char() == '*') {
          read_char();
          if (peek_char() == '/') {
            read_char(true);
            return get_next_token(aliasing);
          }
        }
      } while (peek_char());
    } else {
      if (peek_char() == ' ')
        read_char(true);
      return {token_t::t_divide, "/", "", _line};
    }
    error("Unexpected /");
  }

  if (is_digit(peek_char())) {
    do {
      token_str.push_back(peek_char());
    } while (is_hex(read_char(false)));
    skip_whitespace();
    spdlog::info("get the token str: {}", token_str);

    return {token_t::t_numeric, "numeric", token_str, _line};
  }

  if (peek_char() == '(') {
    read_char(true);
    return {token_t::t_open_bracket, "(", "", _line};
  }

  if (peek_char() == ')') {
    read_char(true);
    return {token_t::t_close_bracket, ")", "", _line};
  }

  if (peek_char() == '{') {
    read_char(true);
    return {token_t::t_open_curly, "{", "", _line};
  }

  if (peek_char() == '}') {
    read_char(true);
    return {token_t::t_close_curly, "}", "", _line};
  }

  if (peek_char() == '[') {
    read_char(true);
    return {token_t::t_open_square, "[", "", _line};
  }

  if (peek_char() == ']') {
    read_char(true);
    return {token_t::t_close_square, "]", "", _line};
  }

  if (peek_char() == ',') {
    read_char(true);
    return {token_t::t_comma, ",", "", _line};
  }

  if (peek_char() == '^') {
    read_char(true);
    return {token_t::t_bit_xor, "^", "", _line};
  }

  if (peek_char() == '~') {
    read_char(true);
    return {token_t::t_bit_not, "~", "", _line};
  }

  if (peek_char() == '"') {
    bool special = false;

    while (read_char(false) != '"' || special) {
      if (token_str.back() == '\\') {
        if (peek_char() == 'n') token_str.back() = '\n';
        else if (peek_char() == '"') token_str.back() = '"';
        else if (peek_char() == 'r') token_str.back() = '\r';
        else if (peek_char() == '\'') token_str.back() = '\'';
        else if (peek_char() == 't') token_str.back() = '\t';
        else if (peek_char() == '\\') token_str.back() = '\\';
        else abort();
      } else {
        token_str.push_back(peek_char());
      }
      if (peek_char() == '\\') special = true;
      else special = false;
    }
    read_char(true);

    return {token_t::t_string, "string", token_str, _line};
  }

  if (peek_char() == '\'') {
    read_char(false);
    if (peek_char() != '"') {
      if (token_str.back() == '\\') {
        if (peek_char() == 'n') token_str.back() = '\n';
        else if (peek_char() == '"') token_str.back() = '"';
        else if (peek_char() == 'r') token_str.back() = '\r';
        else if (peek_char() == '\'') token_str.back() = '\'';
        else if (peek_char() == 't') token_str.back() = '\t';
        else if (peek_char() == '\\') token_str.back() = '\\';
        else abort();
      } else {
        token_str.push_back(peek_char());
      }
    }
    if (read_char() != '\'') abort();
    read_char(true);

    return {token_t::t_char, "char", token_str, _line};
  }

  if (peek_char() == '*') {
    read_char(true);
    return {token_t::t_asterisk, "*", "", _line};
  }

  if (peek_char() == '&') {
    read_char();
    if (peek_char() == '&') {
      read_char(true);
      return {token_t::t_log_and, "&&", "", _line};
    }
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_andeq, "&=", "", _line};
    }
    skip_whitespace();
    return {token_t::t_ampersand, "&", "", _line};
  }

  if (peek_char() == '|') {
    read_char();
    if (peek_char() == '|') {
      read_char(true);
      return {token_t::t_log_or, "||", "", _line};
    }
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_oreq, "|=", "", _line};
    }
    skip_whitespace();
    return {token_t::t_bit_or, "|", "", _line};
  }

  if (peek_char() == '%') {
    read_char(true);
    return {token_t::t_mod, "%", "", _line};
  }

  if (peek_char() == '<') {
    read_char(false);
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_le, "<=", ",", _line};
    }
    if (peek_char() == '<') {
      read_char(true);
      return {token_t::t_lshift, "<<", "", _line};
    }
    skip_whitespace();
    return {token_t::t_lt, "<", "", _line};
  }

  if (peek_char() == '>') {
    read_char(false);
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_ge, ">=", "", _line};
    }
    if (peek_char() == '>') {
      read_char(true);
      return {token_t::t_rshift, ">>", "", _line};
    }
    skip_whitespace();
    return {token_t::t_gt, ">", "", _line};
  }

  if (peek_char() == '!') {
    read_char(false);
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_noteq, "!=", "", _line};
    }
    skip_whitespace();
    return {token_t::t_log_not, "!", "", _line};
  }

  if (peek_char() == '.') {
    read_char();
    if (peek_char() == '.') {
      read_char();
      if (peek_char() == '.') {
        read_char(true);
        return {token_t::t_elipsis, "...", "", _line};
      }
      abort();
    }
    skip_whitespace();
    return {token_t::t_dot, ".", "", _line};
  }

  if (peek_char() == '-') {
    read_char(false);
    if (peek_char() == '>') {
      read_char(true);
      return {token_t::t_arrow, "->", "", _line};
    }
    if (peek_char() == '-') {
      read_char(true);
      return {token_t::t_decrement, "--", "", _line};
    }
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_minuseq, "-=", "", _line};
    }
    skip_whitespace();
    return {token_t::t_minus, "-", "", _line};
  }

  if (peek_char() == '+') {
    read_char();
    if (peek_char() == '+') {
      read_char(true);
      return {token_t::t_increment, "++", "", _line};
    }
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_pluseq, "==", "", _line};
    }
    skip_whitespace();
    return {token_t::t_plus, "+", "", _line};
  }

  if (peek_char() == ';') {
    read_char(true);
    return {token_t::t_semicolon, ";", "", _line};
  }

  if (peek_char() == '?') {
    read_char(true);
    return {token_t::t_question, "?", "", _line};
  }

  if (peek_char() == ':') {
    read_char(true);
    return {token_t::t_colon, ":", "", _line};
  }

  if (peek_char() == '=') {
    read_char();
    if (peek_char() == '=') {
      read_char(true);
      return {token_t::t_eq, "==", "", _line};
    }
    skip_whitespace();
    return {token_t::t_assign, "=", "", _line};
  }

  if (is_alnum(peek_char())) {
    do {
      token_str.push_back(peek_char());
    } while (is_alnum(read_char()));
    skip_whitespace();
    spdlog::info("get the token str: {}", token_str);

    if (token_str == "if")      return {token_t::t_if, token_str, "", _line};
    if (token_str == "while")   return {token_t::t_while, token_str, "", _line};
    if (token_str == "for")     return {token_t::t_for, token_str, "", _line};
    if (token_str == "do")      return {token_t::t_do, token_str, "", _line};
    if (token_str == "else")    return {token_t::t_else, token_str, "", _line};
    if (token_str == "return")  return {token_t::t_return, token_str, "", _line};
    if (token_str == "typedef") return {token_t::t_typedef, token_str, "", _line};
    if (token_str == "enum")    return {token_t::t_enum, token_str, "", _line};
    if (token_str == "struct")  return {token_t::t_struct, token_str, "", _line};
    if (token_str == "sizeof")  return {token_t::t_sizeof, token_str, "", _line};
    if (token_str == "switch")  return {token_t::t_switch, token_str, "", _line};
    if (token_str == "case")    return {token_t::t_case, token_str, "", _line};
    if (token_str == "break")   return {token_t::t_break, token_str, "", _line};
    if (token_str == "contine") return {token_t::t_continue, token_str, "", _line};
    if (token_str == "default") return {token_t::t_default, token_str, "", _line};

    return {token_t::t_identifier, "identifier", token_str, _line};
  }

  // TODO MACRO

  if (_current == _source.size()) return {token_t::t_eof, "eof", "", _line};

  error("Unexpected error");
//  abort();
  return {token_t::t_eof, "eof", "", _line};
}

auto lexemer::read_preproc_directive() -> bool {
  std::string token_str;

  if (peek(token_t::t_cppd_include)) {
    expect(token_t::t_cppd_include);

    if (peek(token_t::t_string)) {
      expect(token_t::t_string);
    } else {
      expect(token_t::t_lt);

      while (!peek(token_t::t_gt)) {
        _next_token = get_next_token();
        spdlog::info("get the next token: {}", me::enum_name(_next_token.token_type));
      }
      expect(token_t::t_gt);
    }
    return true;
  }

  if (accept(token_t::t_cppd_define)) {}
}

auto lexemer::read_global_statement() -> void {

}

auto lexemer::is_alnum(char c) -> bool {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') ||
         (c == '_');
}

auto lexemer::is_digit(char c) -> bool {
  return c >= '0' && c <= '9';
}

auto lexemer::is_hex(char c) -> bool {
  return is_digit(c) ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F') ||
          c == 'x';
}

auto lexemer::is_numeric(const std::string &buf) -> bool {
  bool hex = false;

  if (buf.size() > 2)
    hex = (buf[0] == '0' && buf[1] == 'x');

  for (int32_t i = 0; i < buf.size(); i++) {
    if (hex && !is_hex(buf[i])) return false;
    if (!hex && !is_digit(buf[i])) return false;
  }
  return true;
}

auto lexemer::read_char(bool is_skip_space) -> char {
  _current += 1;
  if (is_skip_space)
    skip_whitespace();
  return _source[_current];
}

auto lexemer::peek_char(int32_t offset) -> char {
  if (_current + offset > _source.size()) {
    spdlog::error("offset over the source size");
    abort();
  }
  return _source[_current + offset];
}

auto lexemer::skip_whitespace() -> void {
  while (true) {
    if (is_linebreak()) {
      _line++;
      _current += 2;
      continue;
    }
    if (is_whitespace()) {
      _current += 1;
      continue;
    } else if ((_skip_newline && is_newline())) {
      _current += 1;
      _line += 1;
      continue;
    }
    break;
  }
}

// BUG maybe
auto lexemer::is_linebreak() -> bool {
  return _source[_current] == '\\' &&
          peek_char(1) == '\n';
}

auto lexemer::is_whitespace() -> bool {
  return _source[_current] == ' ' ||
        _source[_current] == '\t';
}

auto lexemer::is_newline() -> bool {
  return _source[_current] == '\r' ||
        _source[_current] == '\n';
}

auto lexemer::error(const std::string &msg) -> void {
  std::string diagnostic;
  int32_t offset, start = 0;

  for (offset = _current; offset >= 0 && _source[offset] != '\n'; offset--);
  start = offset + 1;

  for (offset = 0; offset < _source.size() && _source[start + offset] != '\n'; offset++);
  diagnostic = _source.substr(start, offset);
  diagnostic += '\n';

  for (int32_t i = _current - start; i > 0; i--)
    diagnostic.push_back(' ');

  diagnostic += "^ Error occurs here";

  spdlog::error("Error {} at source location {} lines\n{}\n", msg, _line, diagnostic);
  abort();
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
