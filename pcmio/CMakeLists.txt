# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2013, All rights reserved.

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(SoundSeparation)

find_package(ALSA REQUIRED)
find_package(ITPP REQUIRED)

add_subdirectory(src) 
add_subdirectory(doc)

