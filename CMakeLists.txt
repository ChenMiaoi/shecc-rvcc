cmake_minimum_required(VERSION 3.0.0)
project(shecc-rvcc VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_COMPILE   "gcc-12")
set(CMAKE_CXX_COMPILE "g++-12")

set(CMAKE_CXX_COMPILE_FEATURES 
  "-pedantic                        \
  -Wall -Wextra                     \
  -Wno-unused-but-set-variable      \
  -Wno-variadic-macros              \
  -Wno-uninitialized                \
  -Wno-strict-prototypes            \
  -Wno-declaration-after-statement  \
  -Wno-format                       \
  -Wno-format-pedantic"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# speed log library
set(DEPS_ROOT ${CMAKE_BINARY_DIR}/deps)
if (NOT EXISTS ${DEPS_ROOT}/spdlog.zip) 
  message(WARNING "spdlog library not exists, start to download")
  file(DOWNLOAD "https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.zip" ${DEPS_ROOT}/spdlog.zip)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${DEPS_ROOT}/spdlog.zip
    WORKING_DIRECTORY ${DEPS_ROOT}
  )
else()
message(STATUS "find spdlog library exists")
endif()

file(GLOB SPDLOG_ROOT_PATH ${DEPS_ROOT}/spdlog*)
list(FILTER SPDLOG_ROOT_PATH EXCLUDE REGEX ".zip$")
get_filename_component(SPDLOG_ROOT ${SPDLOG_ROOT_PATH} NAME)
add_subdirectory(${DEPS_ROOT}/${SPDLOG_ROOT})

include_directories(${DEPS_ROOT}/${SPDLOG_ROOT}/include)

# future we need to add "valgrind" to check memory leek
# find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)

include_directories(include)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cc)
add_library(shecc-rvcca STATIC ${SOURCES})

add_executable(shecc-rvcc main.cc ${SOURCES})

target_link_libraries(shecc-rvcc shecc-rvcca)
