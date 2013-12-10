# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2013, All rights reserved.

add_executable(capture capture.c)
target_link_libraries(capture ${ALSA_LIBRARIES})

add_executable(readdata readdata.c utils.c)
target_link_libraries(readdata ${ALSA_LIBRARIES})

add_executable(convert convert.c utils.c)
target_link_libraries(convert ${ALSA_LIBRARIES})

add_executable(audioica audioica.cpp utils.c)
target_link_libraries(audioica ${ALSA_LIBRARIES} ${ITPP_LIBRARIES})

add_executable(chanfilter chanfilter.c utils.c)
target_link_libraries(chanfilter ${ALSA_LIBRARIES})

