#include <boost/asio.hpp>
#include <cobs/cobs.hpp>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace serial {
class SerialManager {
 public:
  // deny copy
  SerialManager(const SerialManager&) = delete;
  SerialManager& operator=(const SerialManager&) = delete;

  SerialManager(std::string port_name = "") : serial_port_(io_service_) {
    std::string device_paths;
    if (port_name.empty()) {
      auto paths = find_serial_port();
      if (paths.empty()) {
        throw std::runtime_error("No serial devices found");
      }
      device_paths = paths[0];
    } else {
      device_paths = port_name;
    }
    serial_port_.open(device_paths);
    serial_port_.set_option(boost::asio::serial_port_base::baud_rate(115200));
    serial_port_.async_read_some(boost::asio::buffer(&buf, 1),
                                 [this](const boost::system::error_code& ec,
                                        std::size_t bytes_transferred) {
                                   this->serial_callback(ec, bytes_transferred);
                                 });
    read_thread_ = std::thread([this]() { io_service_.run(); });
  }
  ~SerialManager() {
    io_service_.stop();
    if (read_thread_.joinable()) {
      read_thread_.join();
    }
    if (serial_port_.is_open()) {
      serial_port_.close();
    }
  }
  std::vector<uint8_t> get_saved_data() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return saved_data_;
  }

  void send(const std::vector<uint8_t>& data) {
    boost::asio::write(serial_port_, boost::asio::buffer(data));
  }

 private:
  void serial_callback(const boost::system::error_code& ec,
                       std::size_t bytes_transferred) {
    if (ec) {
      std::cerr << "Serial error: " << ec.message() << std::endl;
      return;
    } else {
      if (buf == '\n') {  // end of packet
        auto decoded =
            cobs::decode(received_buffer_.data(), received_buffer_.size());
        received_buffer_.clear();
        if (decoded.empty()) {
          std::cerr << "Decode error: invalid COBS data" << std::endl;
          run_receive();
          return;
        }

        {
          std::lock_guard<std::mutex> lock(data_mutex_);
          saved_data_ = decoded;
        }
      } else {
        received_buffer_.push_back(buf);
      }
      run_receive();
    }
  }
  std::vector<std::string> find_serial_port() {
    std::vector<std::string> device_paths;
    for (const auto& entry : std::filesystem::directory_iterator("/dev"))
      if (entry.is_character_file() || entry.is_block_file() ||
          entry.is_symlink()) {
        std::string filename = entry.path().filename().string();
        if (filename.rfind("ttyACM", 0) == 0)  // パスがttyACMから始まるか
          device_paths.push_back(entry.path().string());
      }
    return device_paths;
  }

  void run_receive() {
    serial_port_.async_read_some(boost::asio::buffer(&buf, 1),
                                 [this](const boost::system::error_code& ec,
                                        std::size_t bytes_transferred) {
                                   serial_callback(ec, bytes_transferred);
                                 });
  }

  boost::asio::io_service io_service_;
  boost::asio::serial_port serial_port_;
  std::thread read_thread_;
  mutable std::mutex data_mutex_;

  uint8_t buf;
  std::vector<uint8_t> received_buffer_;
  std::vector<uint8_t> saved_data_;
};
}  // namespace serial