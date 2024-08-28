cmake_minimum_required(VERSION 3.12)

project(gstream-server)

include_directories(${GSTREAMER_INCLUDES})
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GSTREAMER_RTSP_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME}  PUBLIC ${GST_DEPS})
target_link_libraries(${PROJECT_NAME}  PUBLIC ${OCLEA_PIPELINE_UTIL_DEPS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION usr/bin/app)
