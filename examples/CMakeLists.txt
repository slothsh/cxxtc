# Examples configuration

# Library examples
add_executable(timecode_basic-increment.example timecode_basic-increment.example.cpp)
target_link_libraries(timecode_basic-increment.example PRIVATE magic_enum::magic_enum)
add_executable(timecode_basic-initialization.example timecode_basic-initialization.example.cpp)
target_link_libraries(timecode_basic-initialization.example PRIVATE magic_enum::magic_enum)
