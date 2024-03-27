#ifndef __SHECC_RVCC_LEXEMER_H__
#define __SHECC_RVCC_LEXEMER_H__

#include "defs.h"
#include "global.h"

#include "spdlog/spdlog.h"
#include "magic_enum/magic_enum.hpp"

namespace me = magic_enum;

#include <cstdint>
#include <string>

enum class token_t {
  t_start, /* FIXME: it was intended to start the state machine. */
  t_numeric,
  t_identifier,
  t_comma,  /* , */
  t_string, /* null-terminated string */
  t_char,
  t_open_bracket,  /* ( */
  t_close_bracket, /* ) */
  t_open_curly,    /* { */
  t_close_curly,   /* } */
  t_open_square,   /* [ */
  t_close_square,  /* ] */
  t_asterisk,      /* '*' */
  t_divide,        /* / */
  t_mod,           /* % */
  t_bit_or,        /* | */
  t_bit_xor,       /* ^ */
  t_bit_not,       /* ~ */
  t_log_and,       /* && */
  t_log_or,        /* || */
  t_log_not,       /* ! */
  t_lt,            /* < */
  t_gt,            /* > */
  t_le,            /* <= */
  t_ge,            /* >= */
  t_lshift,        /* << */
  t_rshift,        /* >> */
  t_dot,           /* . */
  t_arrow,         /* -> */
  t_plus,          /* + */
  t_minus,         /* - */
  t_minuseq,       /* -= */
  t_pluseq,        /* += */
  t_oreq,          /* |= */
  t_andeq,         /* &= */
  t_eq,            /* == */
  t_noteq,         /* != */
  t_assign,        /* = */
  t_increment,     /* ++ */
  t_decrement,     /* -- */
  t_question,      /* ? */
  t_colon,         /* : */
  t_semicolon,     /* ; */
  t_eof,           /* end-of-file (EOF) */
  t_ampersand,     /* & */
  t_return,
  t_if,
  t_else,
  t_while,
  t_for,
  t_do,
  t_typedef,
  t_enum,
  t_struct,
  t_sizeof,
  t_elipsis, /* ... */
  t_switch,
  t_case,
  t_break,
  t_default,
  t_continue,
  /* C pre-processor directives */
  t_cppd_include,
  t_cppd_define,
  t_cppd_undef,
  t_cppd_error,
  t_cppd_if,
  t_cppd_elif,
  t_cppd_else,
  t_cppd_endif,
  t_cppd_ifdef
};

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

public:
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

#endif //! __SHECC_RVCC_LEXEMER_H__
