#ifndef SHECC_RVCC_TOKEN_H
#define SHECC_RVCC_TOKEN_H

enum class token_t {
  /* FIXME: it was intended to start the state machine. */
  t_start,      t_eof,
  t_numeric,    t_identifier,   t_string,   t_char,

  t_open_bracket,   t_close_bracket,  /* () */
  t_open_curly,     t_close_curly,    /* {} */
  t_open_square,    t_close_square,   /* [] */
  /* '+' '-' '*' '/' '%' */
  t_plus,       t_minus,    t_asterisk,   t_divide,t_mod,
  /* & | ^ ~ */
  t_ampersand,  t_bit_or,   t_bit_xor,    t_bit_not,
  /* && || ! */
  t_log_and,    t_log_or,   t_log_not,
  /* < > <= >= */
  t_lt,         t_gt,       t_le,t_ge,
  /* << >> */
  t_lshift,     t_rshift,
  /* . -> */
  t_dot,        t_arrow,
  /* -= += |= &= */
  t_minuseq,    t_pluseq,   t_oreq,       t_andeq,
  /* == != = */
  t_eq,         t_noteq,    t_assign,
  /* ++ -- */
  t_increment,  t_decrement,
  /* ? ; : , */
  t_question,   t_colon,    t_semicolon,  t_comma,

  // Key Words
  t_if,         t_else,
  t_while,      t_for,      t_do,
  t_typedef,    t_enum,     t_struct,
  t_sizeof,     t_elipsis, /* ... */
  t_switch,     t_case,     t_default,
  t_break,      t_continue, t_return,

  /* C pre-processor directives */
  t_cppd_include, t_cppd_define,  t_cppd_error,
  t_cppd_if,      t_cppd_elif,    t_cppd_else,    t_cppd_endif,
  t_cppd_ifdef,   t_cppd_undef
};

struct token {
  token_t     token_type;
  std::string lexeme;
  void*       literal;
  int32_t     line;
};

#endif // SHECC_RVCC_TOKEN_H
