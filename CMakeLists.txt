cmake_minimum_required(VERSION 3.9)
project(ffmpeg_lesson_1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_library(ffmpeg INTERFACE)
target_link_libraries(ffmpeg INTERFACE "-framework CoreFoundation"
        "-framework Security"
        "-framework AudioToolbox"
        "-framework VideoToolbox"
        "-framework CoreVideo"
        "-framework CoreMedia"
        )
target_link_libraries(ffmpeg INTERFACE "vpx" "fdk-aac" "bz2" "z" "iconv" "lzma")
target_link_libraries(ffmpeg INTERFACE "mp3lame" "vorbis" "opus" "theora" "x264" "x265" "xvidcore" "vorbisenc" "swresample")
target_link_libraries(ffmpeg INTERFACE
        "/usr/local/lib/libavcodec.a"
        "/usr/local/lib/libavdevice.a"
        "/usr/local/lib/libavformat.a"
        "/usr/local/lib/libavutil.a"
        "/usr/local/lib/libswscale.a"
        "m"
        "pthread")
find_package(OpenCV)
add_subdirectory(examples)
add_subdirectory(simple_test)
add_subdirectory(learning_sdl)
add_subdirectory(output)