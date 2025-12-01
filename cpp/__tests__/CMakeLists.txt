cmake_minimum_required(VERSION 3.14)
project(testcpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)
get_filename_component(TOP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)
set(LIBDATACHANNEL_PATH ${TOP_PATH}/3rdparty/output/unittest/libdatachannel)
set(FFMPEG_PATH ${TOP_PATH}/3rdparty/output/unittest/ffmpeg)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

enable_testing()
find_package(GTest REQUIRED)

file(GLOB CPP_SOURCES
    ${TOP_PATH}/cpp/*.cpp
    ${TOP_PATH}/cpp/FFmpeg/*.cpp
    ${TOP_PATH}/cpp/__tests__/*.cpp
    ${TOP_PATH}/cpp/__tests__/FFmpeg/*.cpp
)

add_executable(
    testcpp
    ${CPP_SOURCES}
)

target_include_directories(
    testcpp
    PUBLIC
    ${TOP_PATH}/cpp/
    ${TOP_PATH}/cpp/FFmpeg/
    SYSTEM
    ${LIBDATACHANNEL_PATH}/include
    ${FFMPEG_PATH}/include
)

target_link_libraries(
    testcpp
    PRIVATE
    ${LIBDATACHANNEL_PATH}/lib/libdatachannel.a
    ${LIBDATACHANNEL_PATH}/lib/libjuice.a
    ${LIBDATACHANNEL_PATH}/lib/libsrtp2.a
    ${LIBDATACHANNEL_PATH}/lib/libusrsctp.a
    ${LIBDATACHANNEL_PATH}/lib/libmbedtls.a
    ${LIBDATACHANNEL_PATH}/lib/libmbedx509.a
    ${LIBDATACHANNEL_PATH}/lib/libmbedcrypto.a
    ${FFMPEG_PATH}/lib/libavformat.a
    ${FFMPEG_PATH}/lib/libavcodec.a
    ${FFMPEG_PATH}/lib/libswscale.a
    ${FFMPEG_PATH}/lib/libswresample.a
    ${FFMPEG_PATH}/lib/libavutil.a
    opus
    x264
    x265
    z
    GTest::gtest
    GTest::gtest_main
)