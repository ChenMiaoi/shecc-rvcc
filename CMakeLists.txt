cmake_minimum_required(VERSION 3.5.0)
project(shecc-rvcc VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_COMPILE   "gcc-11")
set(CMAKE_CXX_COMPILE "g++-11")

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

# target architecture
option(RISCV_ARCH_BUILD "Build for RISC-V architecture" OFF)
if (NOT RISCV_ARCH_BUILD)
  set(ARM_ARCH_BUILD ON)
endif ()

if (ARM_ARCH_BUILD)
  add_compile_definitions(ARCH_PREDEFINED="__arm__")
  add_compile_definitions(ELF_MACHINE=0x28)
  add_compile_definitions(ELF_FLAGS=0x5000200)
endif ()

if (RISCV_ARCH_BUILD)
  add_compile_definitions()
endif ()

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

# magic enum library
if (NOT EXISTS ${DEPS_ROOT}/magic_enum.tar.gz)
  file(DOWNLOAD "https://github.com/Neargye/magic_enum/releases/download/v0.9.5/magic_enum-v0.9.5.tar.gz" ${DEPS_ROOT}/magic_enum.tar.gz)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPS_ROOT}/magic_enum
  )
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar -xzf ${DEPS_ROOT}/magic_enum.tar.gz
    WORKING_DIRECTORY ${DEPS_ROOT}/magic_enum
  )
else ()
endif ()

file(GLOB MAGIC_ENUM_ROOT_PATH ${DEPS_ROOT}/magic_enum*)
list(FILTER MAGIC_ENUM_ROOT_PATH EXCLUDE REGEX ".tar.gz$")
get_filename_component(MAGIC_ENUM_ROOT ${MAGIC_ENUM_ROOT_PATH} NAME)
add_subdirectory(${DEPS_ROOT}/${MAGIC_ENUM_ROOT})

include_directories(${DEPS_ROOT}/${MAGIC_ENUM_ROOT}/include)

# future we need to add "valgrind" to check memory leek
# find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)

include_directories(include)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cc)
add_library(shecc-rvcca STATIC ${SOURCES})

add_executable(shecc-rvcc main.cc ${SOURCES})

target_link_libraries(shecc-rvcc shecc-rvcca)
