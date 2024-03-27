#include "global.h"
#include "defs.h"
#include "detail/__details.h"
#include "detail/elf.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

global_t& gs = global_t::get_instance();

auto global_t::add_type() -> type_t * {
  return &types_source[types_idx++];
}

auto global_t::add_block(block_t *parent, func_t *func, macro_t *macro) -> block_t * {
  block_t* blk = &block_source[blocks_idx];
  blk->index = blocks_idx++;
  blk->parent = parent;
  blk->func = func;
  blk->macro = macro;
  blk->next_local = 0;

  return blk;
}

auto global_t::add_alias(const std::string &alias, const std::string &value) -> void {
  alias_t* al = &aliases_source[aliases_idx++];
  al->alias = alias;
  al->value = value;
  al->disabled = false;
}

auto global_t::add_func(const std::string &name) -> func_t * {
  func_t* fn;
  int32_t index = insert_trie(name.c_str());
  spdlog::info("{}", func_tries_source[8].index);
  if (index == funcs_idx) {
    fn = &funcs_source[funcs_idx++];
    fn->return_def.var_name = name;
  }
  fn = &funcs_source[index];
  fn->stack_size = 4;
  return fn;
}

auto global_t::add_named_type(const std::string &name) -> type_t * {
  type_t* type = add_type();
  type->type_name = name;
  return type;
}

auto global_t::insert_trie(const char* name) -> int32_t {
  return __insert_trie(func_tries_source, name, funcs_idx);
}

auto global_t::__insert_trie(trie_t* trie, const char* name, int32_t funcs_index) -> int32_t {
  int8_t first_char = *name;
  int32_t fc = first_char;

  if (!fc) {
    if (!trie->index)
      trie->index = funcs_index;
    return trie->index;
  }

  if (!trie->next[fc]) {
    trie->next[fc] = func_tries_idx++;
    memset(func_tries_source[trie->next[fc]].next, 0, 128);
    func_tries_source[trie->next[fc]].index = 0;
  }

  return __insert_trie(&func_tries_source[trie->next[fc]], name + 1, funcs_index);
}

static void global_init() {
  global_t& global_source = global_t::get_instance();
  global_source.elf_source.elf_code_start = ELF_START + global_source.elf_source.elf_header_len;
  global_source.elf_source.elf_code     = new int8_t[MAX_CODE];
  global_source.elf_source.elf_header   = new int8_t[MAX_HEADER];
  global_source.elf_source.elf_symtab   = new int8_t[MAX_SYMTAB];
  global_source.elf_source.elf_strtab   = new int8_t[MAX_STRTAB];
  global_source.elf_source.elf_section  = new int8_t[MAX_SECTION];

  global_source.block_source        = new block_t[MAX_BLOCKS];
  global_source.macro_source        = new macro_t[MAX_ALIASES];
  global_source.funcs_source        = new func_t[MAX_FUNCS];
  global_source.func_tries_source   = new trie_t[MAX_FUNC_TRIES];
  global_source.types_source        = new type_t[MAX_TYPES];
  global_source.global_ir_source    = new ph1_ir_t[MAX_GLOBAL_IR];
  global_source.ph1_ir_source       = new ph1_ir_t[MAX_IR_INSTR];
  global_source.ph2_ir_source       = new ph2_ir_t[MAX_IR_INSTR];
  global_source.label_lut_source    = new label_lut_t[MAX_LABEL];
  global_source.source              = new int8_t[MAX_SOURCE]();
  global_source.aliases_source      = new alias_t[MAX_ALIASES];
  global_source.constants_source    = new constant_t[MAX_CONSTANTS];

  global_source.funcs_source[0].stack_size = 4;
}

void global_release() {}

void start() {
  global_init();
}

void error(const std::string& msg) {
  int32_t offset, start_idx, i = 0;
  std::string diagnostic;

  for (offset = gs.source_idx; offset >= 0 && gs.source[offset] != '\n'; offset--);

  start_idx = offset + 1;

  for (offset = 0; offset < MAX_SOURCE && gs.source[start_idx + offset] != '\n'; offset++) {
    diagnostic.push_back(gs.source[start_idx + offset]);
  }
  diagnostic.push_back('\n');

  for (offset = start_idx; offset < gs.source_idx; offset++) {
    diagnostic.push_back(' ');
  }

  diagnostic += "^ Error occus here";

  spdlog::error("Error {} at source location {}\n{}",
                msg, gs.source_idx, diagnostic);
  abort();
}
