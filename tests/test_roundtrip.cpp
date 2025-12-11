#include <gtest/gtest.h>

#include <cobs/cobs.hpp>
#include <cstdint>
#include <random>
#include <vector>

// エンコード→デコードのラウンドトリップテスト
TEST(COBSRoundtrip, BasicRoundtrip) {
  std::vector<uint8_t> original = {0x11, 0x22, 0x00, 0x33, 0x44};

  auto encoded = cobs::encode(original.data(), original.size());
  // エンコードされたデータをそのままデコード
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  EXPECT_EQ(decoded, original);
}

// ランダムデータのラウンドトリップ
TEST(COBSRoundtrip, RandomData) {
  std::mt19937 rng(42);  // 固定シードで再現性を確保
  std::uniform_int_distribution<int> dist(0, 255);

  for (int test = 0; test < 100; ++test) {
    // ランダムな長さのデータを生成（1〜500バイト）
    size_t length = dist(rng) % 500 + 1;
    std::vector<uint8_t> original(length);

    for (size_t i = 0; i < length; ++i) {
      original[i] = static_cast<uint8_t>(dist(rng));
    }

    // エンコード→デコード
    auto encoded = cobs::encode(original.data(), original.size());
    auto decoded = cobs::decode(encoded.data(), encoded.size());

    EXPECT_EQ(decoded, original) << "Failed at test " << test;
  }
}

// 大量のゼロを含むデータ
TEST(COBSRoundtrip, ManyZeros) {
  std::vector<uint8_t> original(100, 0x00);  // 100個のゼロ

  auto encoded = cobs::encode(original.data(), original.size());
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  EXPECT_EQ(decoded, original);
}

// 長いデータ
TEST(COBSRoundtrip, LongData) {
  std::vector<uint8_t> original(1000);
  for (size_t i = 0; i < original.size(); ++i) {
    original[i] = static_cast<uint8_t>(i % 256);
  }

  auto encoded = cobs::encode(original.data(), original.size());
  auto decoded = cobs::decode(encoded.data(), encoded.size());

  // サイズチェック
  if (decoded.size() != original.size()) {
    std::cout << "Size mismatch! decoded: " << decoded.size()
              << ", original: " << original.size() << std::endl;
  }

  // 差分を見つける
  for (size_t i = 0; i < std::min(decoded.size(), original.size()); ++i) {
    if (decoded[i] != original[i]) {
      std::cout << "First difference at index " << i << ": decoded[" << i
                << "] = 0x" << std::hex << (int)decoded[i] << ", original[" << i
                << "] = 0x" << (int)original[i] << std::dec << std::endl;

      // 前後の値も表示
      std::cout << "Context around index " << i << ":" << std::endl;
      for (int j = std::max(0, (int)i - 5);
           j < std::min((int)original.size(), (int)i + 6); ++j) {
        std::cout << "  [" << j << "] decoded: 0x" << std::hex
                  << (int)(j < decoded.size() ? decoded[j] : 0)
                  << ", original: 0x" << (int)original[j] << std::dec
                  << std::endl;
      }
      break;
    }
  }

  EXPECT_EQ(decoded, original);
}
