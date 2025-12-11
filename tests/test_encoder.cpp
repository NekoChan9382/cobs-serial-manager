#include <gtest/gtest.h>

#include <cobs/cobs.hpp>
#include <cstdint>
#include <vector>

// 基本的なエンコードテスト
TEST(COBSEncoder, BasicEncoding) {
  std::vector<uint8_t> data = {0x11, 0x22, 0x00, 0x33};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x03, 0x11, 0x22, 0x02, 0x33, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x03, 0x11, 0x22, 0x02, 0x33, 0x00};
  EXPECT_EQ(encoded, expected);
}

// ゼロバイトなしのデータ
TEST(COBSEncoder, NoZeroBytes) {
  std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x05, 0x11, 0x22, 0x33, 0x44, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x05, 0x11, 0x22, 0x33, 0x44, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 連続するゼロバイト
TEST(COBSEncoder, ConsecutiveZeros) {
  std::vector<uint8_t> data = {0x00, 0x00};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x01, 0x01, 0x01, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x01, 0x01, 0x01, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 先頭がゼロ
TEST(COBSEncoder, LeadingZero) {
  std::vector<uint8_t> data = {0x00, 0x11, 0x00};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x01, 0x02, 0x11, 0x01, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x01, 0x02, 0x11, 0x01, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 末尾がゼロ
TEST(COBSEncoder, TrailingZero) {
  std::vector<uint8_t> data = {0x11, 0x22, 0x00};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x03, 0x11, 0x22, 0x01, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x03, 0x11, 0x22, 0x01, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 空データ
TEST(COBSEncoder, EmptyData) {
  std::vector<uint8_t> data = {};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x01, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x01, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 単一のゼロバイト
TEST(COBSEncoder, SingleZero) {
  std::vector<uint8_t> data = {0x00};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x01, 0x01, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x01, 0x01, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 単一の非ゼロバイト
TEST(COBSEncoder, SingleNonZero) {
  std::vector<uint8_t> data = {0x11};
  auto encoded = cobs::encode(data.data(), data.size());

  // 期待される結果: [0x02, 0x11, 0x00] (末尾デリミタ付き)
  std::vector<uint8_t> expected = {0x02, 0x11, 0x00};
  EXPECT_EQ(encoded, expected);
}

// 254バイトのデータ（セグメントの最大長）
TEST(COBSEncoder, MaxSegmentLength) {
  std::vector<uint8_t> data(254, 0xFF);  // 254個の0xFF
  auto encoded = cobs::encode(data.data(), data.size());

  // 最初のバイトは0xFF（254+1）
  EXPECT_EQ(encoded[0], 0xFF);
  EXPECT_EQ(encoded.size(), 254 + 1 + 1);  // データ + オーバーヘッド + デリミタ
  EXPECT_EQ(encoded.back(), 0x00);         // 最後はデリミタ
}

// 長いデータ（複数セグメント）
TEST(COBSEncoder, LongData) {
  std::vector<uint8_t> data(300, 0x01);  // 300個の0x01
  auto encoded = cobs::encode(data.data(), data.size());

  // エンコード後のサイズを確認
  // 254バイトセグメント + 46バイトセグメント + オーバーヘッド + デリミタ
  EXPECT_GT(encoded.size(), data.size());
  EXPECT_EQ(encoded.back(), 0x00);  // 最後はデリミタ
}
