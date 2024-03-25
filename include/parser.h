#ifndef __SHECC_RVCC_PARSER_H__
#define __SHECC_RVCC_PARSER_H__

#include "global.h"

#include "spdlog/spdlog.h"
#include <string>
class parser {
public:
  parser(const std::string& file);

public:
  void run();

private:
  void load_source();
  void parse_internal();

  void __load_source(const std::string& source_file);
private:
  std::string _file;
};

#endif //! __SHECC_RVCC_PARSER_H__
