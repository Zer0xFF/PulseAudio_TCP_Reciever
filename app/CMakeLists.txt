cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Build the Oboe library
set(OBOE_DIR 3rdparty/oboe)
add_subdirectory(${OBOE_DIR} ./oboe)

# Build our own native library
add_library(native-lib SHARED src/main/cpp/native-lib.cpp src/main/cpp/NativeAudio.cpp src/main/cpp/AudioNet.cpp)
target_link_libraries(native-lib log oboe)
