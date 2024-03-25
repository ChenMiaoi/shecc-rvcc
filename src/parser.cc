#include "parser.h"
#include "global.h"
#include "spdlog/spdlog.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

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

  while (std::getline(file, line, '\r')) {
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
      strcpy((char*)source + source_idx, line.c_str());
      source_idx += line.size();

      spdlog::info("source is: {}\nsource idx is {}", (char*)source, source_idx);
    }
  }

  file.close();
}

void parser::parse_internal() {}