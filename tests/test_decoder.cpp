#include <gtest/gtest.h>

#include <cobs/cobs.hpp>
#include <cstdint>
#include <vector>

// 基本的なデコードテスト
TEST(COBSDecoder, BasicDecoding) {
  std::vector<uint8_t> encoded = {0x03, 0x11, 0x22, 0x02, 0x33, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x11, 0x22, 0x00, 0x33]
  std::vector<uint8_t> expected = {0x11, 0x22, 0x00, 0x33};
  EXPECT_EQ(decoded, expected);
}

// ゼロバイトなしのデータ
TEST(COBSDecoder, NoZeroBytes) {
  std::vector<uint8_t> encoded = {0x05, 0x11, 0x22, 0x33, 0x44, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x11, 0x22, 0x33, 0x44]
  std::vector<uint8_t> expected = {0x11, 0x22, 0x33, 0x44};
  EXPECT_EQ(decoded, expected);
}

// 連続するゼロバイト
TEST(COBSDecoder, ConsecutiveZeros) {
  std::vector<uint8_t> encoded = {0x01, 0x01, 0x01, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x00, 0x00]
  std::vector<uint8_t> expected = {0x00, 0x00};
  EXPECT_EQ(decoded, expected);
}

// 先頭がゼロ
TEST(COBSDecoder, LeadingZero) {
  std::vector<uint8_t> encoded = {0x01, 0x02, 0x11, 0x01, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x00, 0x11, 0x00]
  std::vector<uint8_t> expected = {0x00, 0x11, 0x00};
  EXPECT_EQ(decoded, expected);
}

// 末尾がゼロ
TEST(COBSDecoder, TrailingZero) {
  std::vector<uint8_t> encoded = {0x03, 0x11, 0x22, 0x01, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x11, 0x22, 0x00]
  std::vector<uint8_t> expected = {0x11, 0x22, 0x00};
  EXPECT_EQ(decoded, expected);
}

// 空データ（最小の有効なCOBSデータ）
TEST(COBSDecoder, EmptyData) {
  std::vector<uint8_t> encoded = {0x01, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: []
  std::vector<uint8_t> expected = {};
  EXPECT_EQ(decoded, expected);
}

// 単一のゼロバイト
TEST(COBSDecoder, SingleZero) {
  std::vector<uint8_t> encoded = {0x01, 0x01, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x00]
  std::vector<uint8_t> expected = {0x00};
  EXPECT_EQ(decoded, expected);
}

// 単一の非ゼロバイト
TEST(COBSDecoder, SingleNonZero) {
  std::vector<uint8_t> encoded = {0x02, 0x11, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 期待される結果: [0x11]
  std::vector<uint8_t> expected = {0x11};
  EXPECT_EQ(decoded, expected);
}

// 無効なデータ: 0x00を含む
TEST(COBSDecoder, InvalidDataWithZero) {
  std::vector<uint8_t> encoded = {0x00, 0x11, 0x22, 0x00};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 空のベクトルが返される
  EXPECT_TRUE(decoded.empty());
}

// 無効なデータ: 範囲外参照
TEST(COBSDecoder, InvalidDataOutOfBounds) {
  std::vector<uint8_t> encoded = {0x05, 0x11, 0x22,
                                  0x00};  // 5バイト必要だが3バイトしかない
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 空のベクトルが返される
  EXPECT_TRUE(decoded.empty());
}

// 254バイトセグメント
TEST(COBSDecoder, MaxSegmentLength) {
  std::vector<uint8_t> encoded;
  encoded.push_back(0xFF);  // 254バイトのセグメント
  for (int i = 0; i < 254; ++i) {
    encoded.push_back(0x01);
  }
  encoded.push_back(0x00);  // デリミタ

  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 254個の0x01が返される（ゼロは挿入されない）
  std::vector<uint8_t> expected(254, 0x01);
  EXPECT_EQ(decoded, expected);
}

// 完全な長さ0のデータ
TEST(COBSDecoder, TrulyEmptyInput) {
  std::vector<uint8_t> encoded = {};
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // 空のベクトルが返される（エラーとして扱われる可能性あり）
  EXPECT_TRUE(decoded.empty());
}
