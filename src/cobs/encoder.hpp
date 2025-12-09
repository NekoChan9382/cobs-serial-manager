#ifndef COBS_ENCODER_HPP_
#define COBS_ENCODER_HPP_

#include <cstdint>
#include <vector>

namespace cobs {
std::vector<uint8_t> encode(const uint8_t* data, size_t length) {
  std::vector<uint8_t> encoded;
  encoded.reserve(length + length / 254 + 2);
  uint8_t count = 1;
  constexpr size_t mbs = 0xFF;
  size_t obs_pos = 0;
  for (size_t i = 0; i < length; ++i) {
    if (data[i] == 0x00 || count == mbs) {
      encoded[obs_pos] = count;
      obs_pos = encoded.size();
      encoded.push_back(0x00);
      count = 1;
    } else {
      encoded.push_back(data[i]);
      ++count;
    }
  }
  encoded[obs_pos] = count;
  encoded.push_back(0x00);
  return encoded;
}
}  // namespace cobs

#endif  // COBS_ENCODER_HPP_