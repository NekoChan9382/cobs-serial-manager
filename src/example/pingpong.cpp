#include <chrono>
#include <iostream>
#include <serial/serial_manager.hpp>
#include <thread>

int main() {
  serial::SerialManager serial_manager;
  std::vector<uint8_t> expected = {0x11, 0x22, 0x00, 0x33, 0x44};
  while (true) {
    auto data = serial_manager.get_saved_data();
    if (!data.empty()) {
      std::cout << "Received data: ";
      for (auto byte : data) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
      }
      std::cout << std::dec << std::endl;
    }
    if (data == expected) {
      std::cout << "Received expected pingpong data!" << std::endl;
      serial_manager.send(data);  // エコーバック
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return 0;
}