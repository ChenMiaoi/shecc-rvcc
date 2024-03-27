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

constexpr const uint32_t MAX_FUNCS        = 512;
constexpr const uint32_t MAX_FUNC_TRIES   = 2160;
constexpr const uint32_t MAX_BLOCKS       = 2048;
constexpr const uint32_t MAX_TYPES        = 64;
constexpr const uint32_t MAX_IR_INSTR     = 36864;
constexpr const uint32_t MAX_BB_PRED      = 128;
constexpr const uint32_t MAX_BB_DOM_SUCC  = 64;
constexpr const uint32_t MAX_GLOBAL_IR    = 256;
constexpr const uint32_t MAX_LABEL        = 4096;
constexpr const uint32_t MAX_SOURCE       = 278528;
constexpr const uint32_t MAX_CODE         = 1 << 18;
constexpr const uint32_t MAX_DATA         = 1 << 18;
constexpr const uint32_t MAX_SYMTAB       = 1 << 18;
constexpr const uint32_t MAX_STRTAB       = 1 << 18;
constexpr const uint32_t MAX_HEADER       = 1 << 18;
constexpr const uint32_t MAX_SECTION      = 1 << 18;
constexpr const uint32_t MAX_ALIASES      = 1 << 10;
constexpr const uint32_t MAX_CONSTANTS    = 1 << 10;

constexpr const uint32_t ELF_START        = 0x10000;

template <typename _Ty>
class __singleton {
public:
  static _Ty& get_instance() {
    static _Ty instance;
    return instance;
  }

protected:
  __singleton() = default;
  __singleton(const __singleton&) = delete;
  __singleton& operator= (const __singleton&) = delete;
};


#endif //! __SHECC_RVCC_DEFS_H__
