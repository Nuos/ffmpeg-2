add_executable(example_avio avio_reading.c)
target_link_libraries(example_avio PRIVATE ffmpeg)
add_executable(example_avdir avio_dir_cmd.c)
target_link_libraries(example_avdir PRIVATE ffmpeg)
add_executable(example_encode_audio encode_video.c)
target_link_libraries(example_encode_audio ffmpeg)
add_executable(hello_world 0_hello_world.c)
target_link_libraries(hello_world ffmpeg)
add_executable(highlight  decode_video.c          extract_mvs.c           http_multiclient.c      qsvdec.c                transcode_aac.c
        avio_dir_cmd.c          demuxing_decoding.c     filter_audio.c          hw_decode.c             remuxing.c              transcoding.c
        avio_reading.c          encode_audio.c          filtering_audio.c       metadata.c              resampling_audio.c      vaapi_encode.c
        decode_audio.c          encode_video.c          filtering_video.c       muxing.c                scaling_video.c         vaapi_transcode.c)