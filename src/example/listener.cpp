#include <chrono>
#include <iostream>
#include <serial/serial_manager.hpp>
#include <thread>

int main() {
  serial::SerialManager serial_manager;
  while (true) {
    auto data = serial_manager.get_saved_data();
    if (!data.empty()) {
      std::cout << "Received data: ";
      for (auto byte : data) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
      }
      std::cout << std::dec << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return 0;
}