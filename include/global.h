#ifndef __SHECC_RVCC_GLOBAL_H__
#define __SHECC_RVCC_GLOBAL_H__

#include "defs.h"
#include "detail/elf.h"
#include <cstdint>

// TODO 是否考虑为写为一个类，并为单例模式？

extern elf_t& elf_source;
extern block_t*  block_source;
extern macro_t*  macro_source;
extern func_t*   funcs_source;
extern trie_t*   func_tries_source;
extern type_t*   types_source;
extern ph1_ir_t* global_ir_source;
extern ph1_ir_t* ph1_ir_source;
extern ph2_ir_t* ph2_ir_source;
extern label_lut_t*  label_lut_source;
extern func_list_t   func_lists_source;
extern func_t        global_func_source;
extern basic_block_t* main_bb_source;
extern alias_t*      aliases_source;
extern constant_t*   constants_source;
extern int8_t*       source;

extern int32_t blocks_idx;
extern int32_t macros_idx;
extern int32_t funcs_idx;
extern int32_t func_tries_idx;
extern int32_t types_idx;
extern int32_t global_ir_idx;
extern int32_t ph1_ir_idx;
extern int32_t ph2_ir_idx;
extern int32_t label_lut_idx;
extern int32_t elf_offset;
extern int32_t aliases_idx;
extern int32_t constants_idx;
extern int32_t source_idx;

extern void start();
extern void global_release();

#endif //! __SHECC_RVCC_GLOBAL_H__
