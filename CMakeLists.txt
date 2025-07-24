cmake_minimum_required(VERSION 3.22.1)

project(multimedia-framework)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_compile_options(-g)

find_package(PkgConfig REQUIRED)

pkg_check_modules(ALSA REQUIRED IMPORTED_TARGET alsa)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(DRM REQUIRED IMPORTED_TARGET libdrm)
pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)
# pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-4.0)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(JPEG REQUIRED IMPORTED_TARGET libjpeg)
pkg_check_modules(ONEVPL REQUIRED IMPORTED_TARGET vpl)
pkg_check_modules(OPENGLES REQUIRED IMPORTED_TARGET glesv2)
pkg_check_modules(PULSEAUDIO REQUIRED IMPORTED_TARGET libpulse)
pkg_check_modules(PIPEWIRE REQUIRED IMPORTED_TARGET libpipewire-0.3)
# pkg_check_modules(SRT REQUIRED IMPORTED_TARGET srt)
pkg_check_modules(VA-DRM REQUIRED IMPORTED_TARGET libva-drm)
pkg_check_modules(VULKAN REQUIRED IMPORTED_TARGET vulkan)
# pkg_check_modules(WAVPACK REQUIRED IMPORTED_TARGET wavpack)
pkg_check_modules(WAYLAND-CLIENT REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(WAYLAND-EGL REQUIRED IMPORTED_TARGET wayland-egl)
pkg_check_modules(WAYLAND-CURSOR REQUIRED IMPORTED_TARGET wayland-cursor)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)

add_subdirectory(source)

# target_link_libraries(audio-capture PRIVATE ${PIPEWIRE_LIBRARIES} ${PULSEAUDIO_LIBRARIES})
# target_include_directories(audio-capture PRIVATE ${PIPEWIRE_INCLUDE_DIRS} ${PULSEAUDIO_INCLUDE_DIRS} PUBLIC include)

target_link_libraries(audio-playback PRIVATE ${PIPEWIRE_LIBRARIES} ${PULSEAUDIO_LIBRARIES})
target_include_directories(audio-playback PRIVATE ${PIPEWIRE_INCLUDE_DIRS} ${PULSEAUDIO_INCLUDE_DIRS} PUBLIC include)

# target_link_libraries(video-capture PRIVATE ${DRM_LIBRARIES} ${PIPEWIRE_LIBRARIES} ${XCB_LIBRARIES})
# target_include_directories(video-capture PRIVATE ${DRM_INCLUDE_DIRS} PUBLIC include)

# target_link_libraries(video-encode PRIVATE ${AVCODEC_LIBRARIES} ${AVUTIL_LIBRARIES} ${ONEVPL_LIBRARIES} ${VAAPI_LIBRARIES}
                      # ${GSTREAMER_LIBRARIES})
# target_include_directories(video-encode PRIVATE ${AVCODEC_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${ONEVPL_INCLUDE_DIRS} ${VAAPI_INCLUDE_DIRS}
#                            ${GSTREAMER_INCLUDE_DIRS} PUBLIC include)

target_link_libraries(video-playback PRIVATE ${EGL_LIBRARIES} ${OPENGLES_LIBRARIES} ${VULKAN_LIBRARIES} ${WAYLAND-CLIENT_LIBRARIES}
	                 ${WAYLAND-EGL_LIBRARIES} ${WAYLAND-CURSOR_LIBRARIES} ${XCB_LIBRARIES})
target_include_directories(video-playback PRIVATE ${EGL_INCLUDE_DIRS} ${OPENGLES_INCLUDE_DIRS} ${VULKAN_INCLUDE_DIRS}
	                      ${WAYLAND-CLIENT_INCLUDE_DIRS} ${WAYLAND-EGL_INCLUDE_DIRS} ${WAYLAND-CURSOR_INCLUDE_DIRS} ${XCB_INCLUDE_DIRS})
target_include_directories(video-playback PUBLIC include)
# target_include_directories(video-stream PUBLIC include)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	add_subdirectory(test)

	# target_link_libraries(audio-capture-test PUBLIC audio-capture)
	target_link_libraries(audio-playback-test PUBLIC audio-playback)
	# target_link_libraries(video-capture-test PUBLIC video-capture)
	# target_link_libraries(video-encode-test PUBLIC video-encode)
	target_link_libraries(video-playback-test PUBLIC video-playback)

	# target_link_libraries(video-stream-test PUBLIC video-stream)
endif()
