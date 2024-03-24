#ifndef __SHECC_RVCC_DEFS_H__
#define __SHECC_RVCC_DEFS_H__

#include "detail/__details.h"

#include <cstdint>

enum class base_type_t {
  type_void = 0,
  type_int,
  type_char,
  type_struct,
  type_typedef,
};

enum class opcode_t {};

constexpr const uint32_t MAX_CODE = 262144;
constexpr const uint32_t MAX_DATA = 262144;
constexpr const uint32_t MAX_SYMTAB = 262144;
constexpr const uint32_t MAX_STRTAB = 262144;
constexpr const uint32_t MAX_HEADER = 262144;
constexpr const uint32_t MAX_SECTION = 262144;
constexpr const uint32_t ELF_START = 0x10000;

#endif //! __SHECC_RVCC_DEFS_H__
