#include "global.h"
#include "defs.h"
#include "detail/__details.h"
#include "detail/elf.h"

#include <cstdint>
#include <cstdio>

elf_t& elf_source = elf_t::get_instance();
block_t*  block_source;
macro_t*  macro_source;
func_t*   funcs_source;
trie_t*   func_tries_source;
type_t*   types_source;
ph1_ir_t* global_ir_source;
ph1_ir_t* ph1_ir_source;
ph2_ir_t* ph2_ir_source;
label_lut_t*  label_lut_source;
func_list_t   func_lists_source;
func_t        global_func_source;
basic_block_t* main_bb_source;
alias_t*      aliases_source;
constant_t*   constants_source;
int8_t*       source;

int32_t blocks_idx = 0;
int32_t macros_idx = 0;
int32_t funcs_idx  = 1;
int32_t func_tries_idx = 1;
int32_t types_idx = 0;
int32_t global_ir_idx = 0;
int32_t ph1_ir_idx = 0;
int32_t ph2_ir_idx = 0;
int32_t label_lut_idx = 0;
int32_t elf_offset = 0;
int32_t aliases_idx = 0;
int32_t constants_idx = 0;
int32_t source_idx = 0;

static void global_init() {
  elf_source.elf_code_start = ELF_START + elf_source.elf_header_len;
  elf_source.elf_code     = new int8_t[MAX_CODE];
  elf_source.elf_header   = new int8_t[MAX_HEADER];
  elf_source.elf_symtab   = new int8_t[MAX_SYMTAB];
  elf_source.elf_strtab   = new int8_t[MAX_STRTAB];
  elf_source.elf_section  = new int8_t[MAX_SECTION];

  block_source        = new block_t[MAX_BLOCKS];
  macro_source        = new macro_t[MAX_ALIASES];
  funcs_source        = new func_t[MAX_FUNCS];
  func_tries_source   = new trie_t[MAX_FUNC_TRIES];
  types_source        = new type_t[MAX_TYPES];
  global_ir_source    = new ph1_ir_t[MAX_GLOBAL_IR];
  ph1_ir_source       = new ph1_ir_t[MAX_IR_INSTR];
  ph2_ir_source       = new ph2_ir_t[MAX_IR_INSTR];
  label_lut_source    = new label_lut_t[MAX_LABEL];
  source              = new int8_t[MAX_SOURCE];
  aliases_source      = new alias_t[MAX_ALIASES];
  constants_source    = new constant_t[MAX_CONSTANTS];

  funcs_source[0].stack_size = 4;
}

void global_release() {}

void start() {
  global_init();
}