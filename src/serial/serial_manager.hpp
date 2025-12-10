#include <boost/asio.hpp>
#include <cobs/decoder.hpp>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace serial {
class SerialManager {
 public:
  SerialManager() : serial_port_(io_service_) {
    auto device_paths = find_serial_port();
    if (device_paths.empty()) {
      throw std::runtime_error("No serial devices found");
    }
    serial_port_ = boost::asio::serial_port(io_service_, device_paths[0]);
    serial_port_.set_option(boost::asio::serial_port_base::baud_rate(115200));
    serial_port_.async_read_some(boost::asio::buffer(&buf, 1),
                                 [this](const boost::system::error_code& ec,
                                        std::size_t bytes_transferred) {
                                   this->serial_callback(ec, bytes_transferred);
                                 });
  }
  std::vector<uint8_t> get_saved_data() const { return saved_data_; }

 private:
  void serial_callback(const boost::system::error_code& ec,
                       std::size_t bytes_transferred) {
    if (!ec) {
      if (buf == '\n') {  // end of packet
        auto decoded =
            cobs::decode(received_buffer_.data(), received_buffer_.size());
        saved_data_ = decoded;
        received_buffer_.clear();
      } else {
        received_buffer_.push_back(buf);
      }

      serial_port_.async_read_some(boost::asio::buffer(&buf, 1),
                                   [this](const boost::system::error_code& ec,
                                          std::size_t bytes_transferred) {
                                     serial_callback(ec, bytes_transferred);
                                   });
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

  boost::asio::io_service io_service_;
  boost::asio::serial_port serial_port_;
  uint8_t buf;
  std::vector<uint8_t> received_buffer_;
  std::vector<uint8_t> saved_data_;
};
}  // namespace serial