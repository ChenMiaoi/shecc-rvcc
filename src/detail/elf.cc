#include "detail/elf.h"

auto elf_t::write_int(int8_t *buf, int32_t index, int32_t val) -> int32_t {
  for (int i = 0; i < 4; i++) {
    buf[index++] = extract_byte(val, i);
  }
  return index;
}

auto elf_t::write_header_int(int32_t val) {}

auto elf_t::write_section_int(int32_t val) {}

auto elf_t::write_symbol_int(int32_t val) {
  elf_section_index = write_int(elf_section, elf_section_index, val);
}

auto elf_t::write_code_int(int32_t val) {}

auto elf_t::add_symbol(int8_t *symbol, int32_t len, int32_t pc) -> void {
  write_symbol_int(elf_strtab_index);
}

auto elf_t::extract_byte(int32_t v, int32_t b) -> int8_t {
  return (v >> (b * 8)) & 0xFF;
}
