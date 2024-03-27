#ifndef __SHECC_RVCC_GLOBAL_H__
#define __SHECC_RVCC_GLOBAL_H__

#include "defs.h"
#include "detail/elf.h"
#include "spdlog/spdlog.h"

#include <cstdint>

// TODO 是否考虑为写为一个类，并为单例模式？

struct global_t: public __singleton<global_t> {
  friend __singleton<global_t>;
public:
  elf_t& elf_source = __singleton<elf_t>::get_instance();
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

public:
  auto add_type() -> type_t*;
  auto add_block(block_t* parent, func_t* func, macro_t* macro) -> block_t*;
  auto add_alias(const std::string& alias, const std::string& value) -> void;
  auto add_func(const std::string& name) -> func_t*;
  auto insert_trie(const char* name) -> int32_t;

  auto add_named_type(const std::string& name) -> type_t*;
  auto __insert_trie(trie_t* trie, const char* name, int32_t funcs_index) -> int32_t;

private:
  global_t() = default;
};

extern global_t& gs;

//extern elf_t& elf_source;
//extern block_t*  block_source;
//extern macro_t*  macro_source;
//extern func_t*   funcs_source;
//extern trie_t*   func_tries_source;
//extern type_t*   types_source;
//extern ph1_ir_t* global_ir_source;
//extern ph1_ir_t* ph1_ir_source;
//extern ph2_ir_t* ph2_ir_source;
//extern label_lut_t*  label_lut_source;
//extern func_list_t   func_lists_source;
//extern func_t        global_func_source;
//extern basic_block_t* main_bb_source;
//extern alias_t*      aliases_source;
//extern constant_t*   constants_source;
//extern int8_t*       source;
//
//extern int32_t blocks_idx;
//extern int32_t macros_idx;
//extern int32_t funcs_idx;
//extern int32_t func_tries_idx;
//extern int32_t types_idx;
//extern int32_t global_ir_idx;
//extern int32_t ph1_ir_idx;
//extern int32_t ph2_ir_idx;
//extern int32_t label_lut_idx;
//extern int32_t elf_offset;
//extern int32_t aliases_idx;
//extern int32_t constants_idx;
//extern int32_t source_idx;

extern void start();
extern void global_release();
extern void error(const std::string& msg);

#endif //! __SHECC_RVCC_GLOBAL_H__
