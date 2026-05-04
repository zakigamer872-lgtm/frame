cmake_minimum_required(VERSION 3.16)
project(FrameX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 (fallback to Qt5)
find_package(Qt6 COMPONENTS Widgets Multimedia MultimediaWidgets Network Sql QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Multimedia MultimediaWidgets Network Sql REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Find FFmpeg
find_path(FFMPEG_INCLUDE_DIR libavcodec/avcodec.h
    HINTS $ENV{FFMPEG_DIR}/include
    /usr/include
    /usr/local/include
    "C:/ffmpeg/include"
)
find_library(AVCODEC_LIBRARY avcodec
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)
find_library(AVFORMAT_LIBRARY avformat
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)
find_library(AVUTIL_LIBRARY avutil
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)
find_library(SWSCALE_LIBRARY swscale
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)
find_library(SWRESAMPLE_LIBRARY swresample
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)
find_library(AVDEVICE_LIBRARY avdevice
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)
find_library(AVFILTER_LIBRARY avfilter
    HINTS $ENV{FFMPEG_DIR}/lib /usr/lib /usr/local/lib "C:/ffmpeg/lib"
)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/recording/recorder.cpp
    src/recording/screen_capture.cpp
    src/recording/audio_capture.cpp
    src/recording/webcam_capture.cpp
    src/editor/video_editor.cpp
    src/ui/dashboard_page.cpp
    src/ui/recordings_page.cpp
    src/ui/screenshots_page.cpp
    src/ui/settings_page.cpp
    src/ui/hotkeys_page.cpp
    src/ui/tools_page.cpp
    src/ui/streaming_page.cpp
    src/ui/sidebar_widget.cpp
    src/ui/recording_settings_panel.cpp
    src/ui/floating_widget.cpp
    src/ui/about_dialog.cpp
    src/settings_manager.cpp
    src/hotkey_manager.cpp
    src/history_manager.cpp
    src/theme_manager.cpp
)

set(HEADERS
    src/mainwindow.h
    src/recording/recorder.h
    src/recording/screen_capture.h
    src/recording/audio_capture.h
    src/recording/webcam_capture.h
    src/editor/video_editor.h
    src/ui/dashboard_page.h
    src/ui/recordings_page.h
    src/ui/screenshots_page.h
    src/ui/settings_page.h
    src/ui/hotkeys_page.h
    src/ui/tools_page.h
    src/ui/streaming_page.h
    src/ui/sidebar_widget.h
    src/ui/recording_settings_panel.h
    src/ui/floating_widget.h
    src/ui/about_dialog.h
    src/settings_manager.h
    src/hotkey_manager.h
    src/history_manager.h
    src/theme_manager.h
)

set(RESOURCES
    resources/resources.qrc
)

if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS resources/framex.rc)
    add_executable(FrameX WIN32 ${SOURCES} ${HEADERS} ${RESOURCES} ${APP_ICON_RESOURCE_WINDOWS})
else()
    add_executable(FrameX ${SOURCES} ${HEADERS} ${RESOURCES})
endif()

target_include_directories(FrameX PRIVATE
    src
    ${FFMPEG_INCLUDE_DIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(FrameX PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::Network
        Qt6::Sql
    )
else()
    target_link_libraries(FrameX PRIVATE
        Qt5::Widgets
        Qt5::Multimedia
        Qt5::MultimediaWidgets
        Qt5::Network
        Qt5::Sql
    )
endif()

if(AVCODEC_LIBRARY)
    target_link_libraries(FrameX PRIVATE
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY}
        ${SWRESAMPLE_LIBRARY}
        ${AVDEVICE_LIBRARY}
        ${AVFILTER_LIBRARY}
    )
    target_compile_definitions(FrameX PRIVATE FFMPEG_AVAILABLE)
endif()

if(WIN32)
    target_link_libraries(FrameX PRIVATE
        dwmapi
        d3d11
        dxgi
        gdi32
        user32
        winmm
    )
endif()

# Install rules
install(TARGETS FrameX
    RUNTIME DESTINATION bin
)

set_target_properties(FrameX PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "FrameX"
)
