add_executable(audio simplest_ffmpeg_audio_player.cpp)

target_link_libraries(audio "-framework CoreFoundation"
        "-framework Security"
        "-framework AudioToolbox"
        "-framework VideoToolbox"
        "-framework CoreVideo"
        "-framework CoreMedia"
        )
target_link_libraries(audio "vpx" "fdk-aac" "bz2" "z" "iconv" "lzma" "SDL2")
link_directories("/usr/lib")
link_directories("/usr/local/lib")
target_link_libraries(audio "mp3lame" "vorbis" "opus" "theora" "x264" "x265" "xvidcore" "vorbisenc" "swresample")
target_link_libraries(audio
        "/usr/local/lib/libavcodec.a"
        "/usr/local/lib/libavdevice.a"
        "/usr/local/lib/libavformat.a"
        "/usr/local/lib/libavutil.a"
        "m"
        "pthread")
