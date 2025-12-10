find_package(Boost REQUIRED COMPONENTS system)

set(EXAMPLES listener pingpong)

foreach(example ${EXAMPLES})
add_executable(${example} ${example}.cpp)

target_link_libraries(${example} PRIVATE
  cobs-serial-manager
  Boost::system
  pthread
)

endforeach()