cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME visaudio)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

project(${PROJECT_NAME})


set(SOURCE_FILES src/main.c src/ringbuffer.c)
add_executable(visaudio ${SOURCE_FILES})
target_link_libraries(visaudio PUBLIC raylib)