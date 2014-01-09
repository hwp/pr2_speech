# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2013, All rights reserved.

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(SoundSeparation)

find_package(ALSA REQUIRED)
include_directories(${ALSA_INCLUDE_DIRS})

find_package(ITPP REQUIRED)
include_directories(${ITPP_INCLUDE_DIRS})

find_package(GStreamer REQUIRED)
include_directories(${GSTREAMER_INCLUDE_DIRS})

find_package(GLIB COMPONENTS gobject REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

add_subdirectory(src) 
add_subdirectory(doc)

