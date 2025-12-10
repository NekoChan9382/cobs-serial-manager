cmake_minimum_required(VERSION 3.14)
project(cobs-serial-manager VERSION 1.0.0 LANGUAGES CXX)

# C++17を要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# コンパイラ警告を有効化
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ヘッダオンリーライブラリとしてインターフェースライブラリを定義
add_library(cobs-serial-manager INTERFACE)
target_include_directories(cobs-serial-manager INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

option(BUILD_EXAMPLES "Build example applications" ON)
if(BUILD_EXAMPLES)
  add_subdirectory(src/example)
endif()

# テストを有効化
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# インストール設定
install(DIRECTORY src/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp"
)
