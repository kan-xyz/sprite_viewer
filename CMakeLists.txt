cmake_minimum_required(VERSION 3.0)
project(SFML_sprite_viewer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#
# --------------------------------------------------------------------------------------------
# VARIABLES
# --------------------------------------------------------------------------------------------
#
# NOTE:
# you need to set the variables SFML_INCLUDE_DIR and SFML_LIB_DIR before you can
# compile anything in this project
#
# SFML_INCLUDE_DIR = the path to the SFML include folder
# SFML_LIB_DIR = the path to the lilbrary directories for SFML
#
# --------------------------------------------------------------------------------------------
#

set(SFML_INCLUDE_DIR C:/Users/Carl/Desktop/libraries/SFML-2.5.1/include)
set(SFML_LIB_DIR C:/Users/Carl/Desktop/libraries/SFML-2.5.1/lib)

add_compile_definitions(SFML_STATIC)
add_executable(SpriteViewer ${CMAKE_SOURCE_DIR}/sprite-viewer.cpp ${CMAKE_SOURCE_DIR}/main.cpp)
target_include_directories(SpriteViewer PUBLIC ${SFML_INCLUDE_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_link_libraries(
            SpriteViewer
            ${SFML_LIB_DIR}/sfml-graphics-s-d.lib
            ${SFML_LIB_DIR}/sfml-window-s-d.lib
            ${SFML_LIB_DIR}/sfml-audio-s-d.lib
            ${SFML_LIB_DIR}/sfml-system-s-d.lib
            ${SFML_LIB_DIR}/sfml-network-s-d.lib
            ${SFML_LIB_DIR}/flac.lib
            ${SFML_LIB_DIR}/freetype.lib
            ${SFML_LIB_DIR}/ogg.lib
            ${SFML_LIB_DIR}/openal32.lib
            ${SFML_LIB_DIR}/vorbis.lib
            ${SFML_LIB_DIR}/vorbisenc.lib
            ${SFML_LIB_DIR}/vorbisfile.lib
            opengl32.lib
            winmm.lib
            gdi32.lib
        )
    elseif(MINGW)
    endif()
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
    elseif(MINGW)
    endif()
endif()
