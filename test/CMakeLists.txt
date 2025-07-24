# add_executable(audio-capture-test audio-capture-test.c)
# set_target_properties(audio-capture-test PROPERTIES OUTPUT_NAME audio-capture)

add_executable(audio-playback-test audio-playback-test.c)
set_target_properties(audio-playback-test PROPERTIES OUTPUT_NAME audio-playback)

# add_executable(video-capture-test video-capture-test.c)
# set_target_properties(video-capture-test PROPERTIES OUTPUT_NAME video-capture)

# add_executable(video-encode-test video-encode-test.c)
# set_target_properties(video-encode-test PROPERTIES OUTPUT_NAME video-encode)

add_executable(video-playback-test video-playback-test.c)
set_target_properties(video-playback-test PROPERTIES OUTPUT_NAME video-playback)

# add_executable(video-stream-test video-stream-test.c)
# set_target_properties(video-stream-test PROPERTIES OUTPUT_NAME
# 	video-stream)
