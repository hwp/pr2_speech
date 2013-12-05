# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2013, All rights reserved.

add_executable(capture capture.c)
target_link_libraries(capture asound)

add_executable(readdata readdata.c utils.c)
target_link_libraries(readdata asound)

add_executable(convert convert.c utils.c)
target_link_libraries(convert asound)

add_executable(audioica audioica.cpp utils.c)
target_link_libraries(audioica asound itpp)

