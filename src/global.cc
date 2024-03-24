#include "global.h"

elf_t& elf_source = elf_t::get_instance();

static void global_init() {
  elf_source.elf_code_start = ELF_START + elf_source.elf_header_len;
  elf_source.elf_code = new int8_t[MAX_CODE];
  elf_source.elf_header = new int8_t[MAX_HEADER];
  elf_source.elf_symtab = new int8_t[MAX_SYMTAB];
  elf_source.elf_strtab = new int8_t[MAX_STRTAB];
  elf_source.elf_section = new int8_t[MAX_SECTION];
}

void start() {
  global_init();
}