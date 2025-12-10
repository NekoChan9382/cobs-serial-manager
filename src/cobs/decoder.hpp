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

inline std::vector<uint8_t> encode(const uint8_t* data, size_t length) {
  std::vector<uint8_t> encoded;
  encoded.reserve(length + length / 254 + 2);  // allocate memory
  uint8_t count = 1;
  constexpr size_t mbs = 0xFF;
  size_t obs_pos = 0;

  encoded.push_back(0x00);
  for (size_t i = 0; i < length; ++i) {
    if (data[i] == 0x00) {
      encoded[obs_pos] = count;
      obs_pos = encoded.size();
      encoded.push_back(0);  // placeholder for next overhead byte
      count = 1;
    } else {
      if (count == mbs) {
        encoded[obs_pos] = count;
        obs_pos = encoded.size();
        encoded.push_back(0);  // placeholder for next overhead byte
        count = 1;
      }
      encoded.push_back(data[i]);
      ++count;
    }
  }
  encoded[obs_pos] = count;
  encoded.push_back(0x00);  // final overhead byte
  return encoded;
}
}  // namespace cobs
#endif  // COBS_DECODER_HPP_