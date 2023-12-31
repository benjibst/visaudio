cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME visaudio)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
project(${PROJECT_NAME})

set(SOURCE_FILES src/ringbuffer.cpp src/dft.cpp src/main.cpp)
add_executable(visaudio ${SOURCE_FILES})
set_property(TARGET visaudio PROPERTY C_STANDARD 11)
if(MSVC)
  target_include_directories(visaudio PUBLIC "C:\\dev\\raylib\\build\\raylib\\include")
  target_link_directories(visaudio PUBLIC "C:\\dev\\raylib\\build\\raylib\\Release")
  target_link_libraries(visaudio PUBLIC raylib.lib winmm.lib)
  target_compile_options(visaudio PRIVATE /W4 /WX)
else()
    target_link_libraries(visaudio PUBLIC raylib m)
    target_compile_options(visaudio PRIVATE -Wall -Wextra -Wpedantic -Werror) #and treat all warnings as errors 
endif()

