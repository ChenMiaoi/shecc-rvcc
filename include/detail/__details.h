#ifndef __SHECC_RVCC_DETAILS_H__
#define __SHECC_RVCC_DETAILS_H__

#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

enum class opcode_t;
enum class base_type_t;

// #define gen_instance_code(name)   \
//   static name& get_##name##_instance() {   \
//     static name instance;         \
//     return instance;              \
//   }                               \
//                                   \
//   name(const name&) = delete;     \
//   name& operator= (const name&) = delete; \
//                                   \
// private:                          \
//   name() = default;               \
//   ~name() = default;              \

struct rename_t {
  int32_t counter;
  std::vector<int32_t> stack;
  int32_t stack_idx;
};

struct ref_block_t;

struct ref_block_list_t {
  ref_block_t* head;
  ref_block_t* tail;
};

struct var_t {
  std::string type_name;
  std::string var_name;
  bool is_ptr;
  bool is_func;
  bool is_global;
  uint32_t array_size;
  int32_t offset;
  int32_t init_val;
  int32_t live_ness;
  bool in_loop;

  var_t* base;
  int32_t sub_script;
  std::vector<var_t*> sub_scrpts;
  int32_t sub_scripts_idx;

  rename_t rename;
  ref_block_list_t ref_block_list;
  int32_t consumed;
  bool is_ternary_ret;
  bool is_constl;

};

struct macro_t {
  std::string name;
  bool is_variadic;
  int32_t start_source_idx;
  std::vector<var_t> param_defs;
  int32_t param_defs_num;
  std::vector<int32_t> params;
  int32_t params_num;
  bool disabled;

};

struct fn_t;

struct func_t {
  var_t return_def;
  std::vector<var_t> params_defs;
  int32_t params_num;
  int32_t va_args;
  int32_t stack_size;
  fn_t* fn;
};

struct block_t {
  std::vector<var_t> locals;
  int32_t next_local;
  block_t* parent;
  func_t* func;
  macro_t* macro;
  int32_t locals_size;
  int32_t index;
};

struct symbol_list_t;
struct basic_block_t;
struct symbol_t;

struct symbol_list_t {
  symbol_t* head;
  symbol_t* tail;
};

struct ph1_ir_t {
  opcode_t op;
  std::string func_name;
  int32_t param_num;
  int32_t size;
  var_t* dest;
  var_t* src0;
  var_t* src1;
};

struct label_lut_t {
  std::string name;
  int32_t offset;
};

struct ph2_ir_t {
  opcode_t op;
  int32_t src0;
  int32_t scr1;
  int32_t dest;
  std::string func_name;
  basic_block_t* next_bb;
  basic_block_t* then_bb;
  basic_block_t* else_bb;
  ph2_ir_t* next;
  bool is_branch_detached;
};

struct type_t {
  std::string type_name;
  base_type_t base_type;
  type_t* base_struct;
  int32_t size;
  std::vector<var_t> fields;
  int32_t fields_num;

  auto add_named_type(const std::string& name) -> type_t*;
};

struct lvalue_t {
  int32_t size;
  bool is_ptr;
  bool is_func;
  bool is_reference;
  type_t* type;
};

struct alias_t {
  std::string alias;
  std::string value;
  bool disabled;
};

struct constant_t {
  std::string alias;
  int32_t value;
};

struct trie_t {
  int32_t index;
  int8_t  next[128] {};
};

struct phi_operand_t {
  var_t* var;
  basic_block_t* from;
  phi_operand_t* next;
};

struct insn_t {
  insn_t* next;
  insn_t* prev;
  int32_t idx;
  opcode_t opcode;
  var_t* rd;
  var_t* rs1;
  var_t* rs2;
  int32_t sz;
  phi_operand_t* phi_ops;
  std::string str;
};

struct insn_list_t {
  insn_t* head;
  insn_t* tail;
};

struct ph2_ir_list_t {
  ph2_ir_list_t* head;
  ph2_ir_list_t* next;
};

enum class bb_connection_type_t {
  NEXT, ELSE, THEN
};

struct bb_connection_t {
  basic_block_t* bb;
  bb_connection_type_t type;
};

struct symbol_t {
  var_t* var;
  int32_t index;
  symbol_t* next;
};

struct basic_block_t {
  insn_list_t insn_list;
  ph2_ir_list_t ph2_ir_list;
  std::vector<bb_connection_t> prev;

  basic_block_t* next;
  basic_block_t* then_;
  basic_block_t* else_;
  basic_block_t* idom;
  basic_block_t* rpo_next;
  basic_block_t* rpo_r_next;

  std::vector<var_t*> live_gen;
  int32_t live_gen_idx;

  std::vector<var_t*> live_kill;
  int32_t live_kill_idx;

  std::vector<var_t*> live_in;
  int32_t live_in_idx;

  std::vector<var_t*> live_out;
  int32_t live_out_idx;

  int32_t rpo;
  int32_t rpo_r;
  std::vector<basic_block_t*> DF;
  int32_t df_idx;
  int32_t visited;

  std::vector<basic_block_t*> dom_next;
  std::vector<basic_block_t*> dom_prev;
  fn_t* belong_to;
  block_t* scope;
  symbol_list_t symbol_list;
  int32_t elf_offset;
};

struct ref_block_t {
  basic_block_t* bb;
  ref_block_t* next;
};

struct fn_t {
  basic_block_t* bbs;
  basic_block_t* exit;
  symbol_list_t global_sym_list;
  int32_t bb_count;
  int32_t visited;
  func_t* func;
  fn_t* next;
};

struct func_list_t {
  fn_t* head;
  fn_t* next;
};

struct bb_traversal_args_t {
  fn_t* fn;
  basic_block_t* bb;
  std::function<void(fn_t*, basic_block_t*)> preorder_cb;
  std::function<void(fn_t*, basic_block_t*)> postorder_cb;
};

struct regfile_t {
  var_t* var;
  int32_t polluted;
};

#endif //! __SHECC_RVCC_DETAILS_H__
