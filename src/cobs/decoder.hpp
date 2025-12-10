#ifndef COBS_DECODER_HPP_
#define COBS_DECODER_HPP_
#include <cstdint>
#include <vector>
namespace cobs {
inline std::vector<uint8_t> decode(const uint8_t* data, size_t length) {
  if (length == 0) {
    return {};
  }
  std::vector<uint8_t> decoded;
  decoded.reserve(length);  // allocate memory
  constexpr size_t mbs = 0xFF;
  const size_t trimmed_length = length - 1;
  uint8_t delimiter = data[trimmed_length];
  std::cout << "Delimiter: " << std::hex << static_cast<int>(delimiter)
            << std::dec << std::endl;
  size_t i = 0;
  while (i < trimmed_length) {  // Last byte is delimiter
    uint8_t code = data[i];
    if (code == delimiter) {
      // Invalid COBS encoded data
      return {};
    }
    const size_t obh = code - 1;
    if (i + code > trimmed_length) {
      // Invalid COBS encoded data
      return {};
    }
    for (size_t j = 0; j < obh; ++j) {
      decoded.push_back(data[i + 1 + j]);
    }
    i += code;
    if (code < mbs && i < trimmed_length) {
      decoded.push_back(delimiter);
    }
  }
  return decoded;
}
}  // namespace cobs
#endif  // COBS_DECODER_HPP_