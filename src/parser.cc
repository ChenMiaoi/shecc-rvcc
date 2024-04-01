#include "parser.h"
#include "global.h"
#include "spdlog/spdlog.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#if 0
parser::parser(const std::string& file)
  : _file {file} {
  spdlog::info("parser get file: {}", _file);
}

void parser::run() {
  load_source();
  parse_internal();
}

void parser::load_source() {
  __load_source(_file);
}

void parser::__load_source(const std::string& source_file) {
  std::string line;

  fs::path file_path = fs::canonical(source_file);

  if (!std::filesystem::exists(file_path)) {
    spdlog::error("\"{}\" file not exists!", source_file);
    abort();
  }

  std::ifstream file(file_path);

  if (!file.is_open()) {
    spdlog::error("\"{}\" file cannot open!", source_file);
    abort();
  }
  spdlog::info("open \"{}\" file.", source_file);

  while (std::getline(file, line)) {
    if (!line.compare(0, 9, "#include ") && line[9] == '"') {
      spdlog::info("matched include.");

      std::string path = file_path.parent_path();
      // if (_file.rfind('/') != std::string::npos) {
      //   path = _file.substr(_file.rfind('/') + 1);
      // } else {
      //   path = _file;
      // }
      spdlog::info("get the file directory: {}", path);

      path += '/';
      path += line.substr(
        line.find('"') + 1,
        line.rfind('"') - line.find('"') - 1
      );
      spdlog::info("get the source file include header path: {}", path);

      __load_source(path);
    } else {
      strcpy((char*)gs.source + gs.source_idx, line.c_str());
      gs.source_idx += line.size();
      gs.source[gs.source_idx++] = '\n';
    }
  }

  spdlog::info("source is: \n{}\nsource idx is {}", (char*)gs.source, gs.source_idx);
  file.close();
}

void parser::parse_internal() {
  type_t* type = gs.add_named_type("void");
  type->base_type = base_type_t::type_void;
  type->size = 0;

  spdlog::info("add the named type: {}, type size: {}, types index: {}",
               type->type_name, type->size, gs.types_idx);

  type = gs.add_named_type("char");
  type->base_type = base_type_t::type_char;
  type->size = 1;

  spdlog::info("add the named type: {}, type size: {}, types index: {}",
               type->type_name, type->size, gs.types_idx);

  type = gs.add_named_type("int");
  type->base_type = base_type_t::type_int;
  type->size = 4;

  spdlog::info("add the named type: {}, type size: {}, types index: {}",
               type->type_name, type->size, gs.types_idx);

  static_cast<void>(gs.add_block(nullptr, nullptr, nullptr));
  gs.elf_source.add_symbol((int8_t *)"", 0, 0);

  gs.add_alias(ARCH_PREDEFINED, "1");
  gs.add_alias("__SHECC__", "1");

  func_t* func = gs.add_func("__syscall");
  func->params_num = 0;
  func->va_args = 1;
  func->fn = new fn_t();
  func->fn->bbs = new basic_block_t();

  gs.source_idx = 0;
  lx.next_char = gs.source[0];
  lx.expect(token_t::t_start);

  do {
    if (lx.read_preproc_directive())
      continue;
    lx.read_global_statement();
  } while (!lx.accept(token_t::t_eof));
}
#endif
