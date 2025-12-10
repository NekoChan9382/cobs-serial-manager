find_package(Boost REQUIRED COMPONENTS system)

add_executable(listener serial_decode.cpp)

target_link_libraries(listener PRIVATE
  cobs-serial-manager
  Boost::system
  pthread
)