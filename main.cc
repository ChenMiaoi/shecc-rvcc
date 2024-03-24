#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#include "global.h"

#include "spdlog/common.h"
#include "spdlog/spdlog.h"

auto main(int argc, char* argv[]) -> int {
	spdlog::set_level(spdlog::level::debug);

	bool libc = true;
	std::string in, out;

	for (int32_t i = 1; i < argc; i++) {
		if (std::string_view(argv[i]) == "--no-libc") {
			libc = false;
		} else if (std::string_view(argv[i]) == "-o") {
			if (i < argc + 1) {
				out = std::string_view(argv[i + 1]);
				i = i + 1;
				spdlog::info("output binary: {}", out);
			} else {
				// error
				spdlog::error("Unsupported options!");
				abort();
			}
		} else {
			in = std::string_view(argv[i]);
			spdlog::info("source file: {}", in);
		}
	}

	if (in.empty()) {
		spdlog::error("Missing source file!");
		spdlog::error("Usage: shecc-rvcc [-o output] <input.c>");
		return -1;
	}
 
	start();

	// if (libc) libc_generate();
  return 0;
}
