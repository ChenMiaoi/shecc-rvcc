#ifndef __SHECC_RVCC_ELF_H__
#define __SHECC_RVCC_ELF_H__

#include <cstdint>

struct elf_t {
  int8_t* elf_code;           // ELF文件中代码段的指针
  int32_t elf_code_idx;       // 代码段的索引或偏移量

  int8_t* elf_data;           // ELF文件中数据段的指针
  int32_t elf_data_idx;       // 数据段的索引或偏移量

  int8_t* elf_header;         // ELF文件头部信息的指针
  int32_t elf_header_idx;     // ELF文件头部的索引或偏移量
  int32_t elf_header_len;     // ELF文件头部的长度

  int32_t elf_code_start;     // 代码段的起始位置
  int32_t elf_data_start;     // 数据段的起始位置

  int8_t* elf_symtab;         // ELF文件的符号表的指针
  int8_t* elf_strtab;         // ELF文件的字符串表的指针
  int8_t* elf_section;        // ELF文件中节表的指针

  static elf_t& get_instance() {
    static elf_t instance;
    return instance;
  }

  elf_t(const elf_t&) = delete;
  elf_t& operator= (const elf_t&) = delete;

private:
  elf_t() = default;
  ~elf_t() = default;
};




#endif //! __SHECC_RVCC_ELF_H__
