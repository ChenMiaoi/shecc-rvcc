#ifndef __SHECC_RVCC_ELF_H__
#define __SHECC_RVCC_ELF_H__

#include "defs.h"

#include <cstdint>

struct elf_t: public __singleton<elf_t> {
  friend __singleton<elf_t>;

  int8_t* elf_code;               // ELF文件中代码段的指针
  int32_t elf_code_idx = 0;       // 代码段的索引或偏移量

  int8_t* elf_data;               // ELF文件中数据段的指针
  int32_t elf_data_idx;           // 数据段的索引或偏移量

  int8_t* elf_header;             // ELF文件头部信息的指针
  int32_t elf_header_idx = 0;     // ELF文件头部的索引或偏移量
  int32_t elf_header_len = 0x54;  // ELF文件头部的长度

  int32_t elf_code_start;         // 代码段的起始位置
  int32_t elf_data_start;         // 数据段的起始位置

  int8_t* elf_symtab;             // ELF文件的符号表的指针
  int32_t elf_symtab_index;
  int32_t elf_symbol_index;
  int8_t* elf_strtab;             // ELF文件的字符串表的指针
  int32_t elf_strtab_index;
  int8_t* elf_section;            // ELF文件中节表的指针
  int32_t elf_section_index;

public:
  auto add_symbol(int8_t* symbol, int32_t len, int32_t pc) -> void;

  auto write_int(int8_t* buf, int32_t index, int32_t val) -> int32_t;
  auto write_header_int(int32_t val);
  auto write_section_int(int32_t val);
  auto write_symbol_int(int32_t val);
  auto write_code_int(int32_t val);

  auto extract_byte(int32_t v, int32_t b) -> int8_t;

private:
  elf_t() = default;
};




#endif //! __SHECC_RVCC_ELF_H__
