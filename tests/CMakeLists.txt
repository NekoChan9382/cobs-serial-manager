include(FetchContent)

# Google Testをダウンロード
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# Windows環境での問題を回避
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# テスト実行可能ファイルを作成
add_executable(cobs_tests
  test_encoder.cpp
  test_decoder.cpp
  test_roundtrip.cpp
)

target_link_libraries(cobs_tests
  PRIVATE
    cobs-serial-manager
    GTest::gtest_main
)

# Google Testのテストを自動検出
include(GoogleTest)
gtest_discover_tests(cobs_tests)
