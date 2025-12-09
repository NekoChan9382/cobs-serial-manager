#ifndef COBS_DECODER_HPP_
#define COBS_DECODER_HPP_
#include <cstdint>
#include <vector>
namespace cobs {
std::vector<uint8_t> decode(const uint8_t* data, size_t length) {
  std::vector<uint8_t> decoded;
  decoded.reserve(length);  // allocate memory
  constexpr size_t mbs = 0xFF;
  size_t i = 0;
  while (i < length) {
    uint8_t code = data[i];
    if (code == 0x00) {
      // Invalid COBS encoded data
      return {};
    }
    const size_t obh = code - 1;
    if (i + code > length) {
      // Invalid COBS encoded data
      return {};
    }
    for (size_t j = 0; j < obh; ++j) {
      decoded.push_back(data[i + 1 + j]);
    }
    i += code;
    if (code < mbs && i < length) {
      decoded.push_back(0x00);
    }
  }
  return decoded;
}
}  // namespace cobs
#endif  // COBS_DECODER_HPP_