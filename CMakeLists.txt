cmake_minimum_required(VERSION 3.5)

project(simple-project)

add_executable(${PROJECT_NAME} main.cpp)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION usr/bin/app)
