# add_library(audio-capture STATIC audio-capture.c audio-capture-pipewire.c audio-capture-pulseaudio.c)

add_library(audio-playback STATIC audio-playback.c audio-playback-pipewire.c audio-playback-pulseaudio.c)

# add_library(audio-codec SHARED audio-codec.c)

# add_library(audio-playback SHARED audio-playback.c)

# add_library(audio-stream SHARED audio-stream.c)

# add_library(video-capture STATIC video-capture.c video-capture-drm.c)

# add_library(video-codec SHARED video-codec.c video-codec-ffmpeg.c video-codec-onevpl.c)

# add_library(video-codec STATIC video-codec.c video-codec-ffmpeg.c video-codec-gstreamer.c video-codec-onevpl.c video-codec-turbojpeg.c)

add_library(video-playback STATIC
	video-playback.c
	video-playback-render.c
	video-playback-render-opengles.c
	video-playback-render-vulkan.c
	# video-playback-egl.c
	# video-playback-opengles.c
	# video-playback-vulkan.c
	video-playback-wayland.c
	video-playback-xcb.c
	wayland-protocols/presentation-time-client-protocol.c
	wayland-protocols/viewporter-client-protocol.c
	wayland-protocols/xdg-shell-client-protocol.c)

# add_library(video-stream
# 	SHARED
# 	video-stream.c)

# ```
# wayland-scanner private-code < /usr/share/wayland-protocols/stable/viewporter/viewporter.xml > source/viewporter-client-protocol.c
# wayland-scanner client-header < /usr/share/wayland-protocols/stable/viewporter/viewporter.xml > source/viewporter-client-protocol.h
# ```
# ```
# wayland-scanner private-code < /usr/share/wayland-protocols/stable/presentation-time/presentation-time.xml > source/presentation-time-client-protocol.c
# wayland-scanner client-header < /usr/share/wayland-protocols/stable/presentation-time/presentation-time.xml > source/presentation-time-client-protocol.h
# ```
# ```
# wayland-scanner private-code < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > source/xdg-shell-client-protocol.c
# wayland-scanner client-header < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > source/xdg-shell-client-protocol.h
# ```
# ```
# wayland-scanner private-code < /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml > source/xdg-decoration-unstable-v1-client-protocol.c
# wayland-scanner client-header < /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml > source/xdg-decoration-unstable-v1-client-protocol.h
# ```


# find_package(QT
# 	NAMES
# 	Qt6
# 	REQUIRED COMPONENTS
# 	Core
# 	OpenGL
# 	Widgets
# 	OpenGLWidgets)

# find_package(Qt${QT_VERSION_MAJOR}
# 	REQUIRED COMPONENTS
# 	Core
# 	OpenGL
# 	Widgets
# 	OpenGLWidgets)

# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(source)

# target_link_libraries(video-playback-qt
# 	PRIVATE
# 	Qt${QT_VERSION_MAJOR}::Core
# 	Qt${QT_VERSION_MAJOR}::OpenGL
# 	Qt${QT_VERSION_MAJOR}::Widgets
# 	Qt${QT_VERSION_MAJOR}::OpenGLWidgets rt)

# target_include_directories(video-playback-qt
#         PUBLIC
#         # ${QT_INCLUDE_DIRS}
#         include)
