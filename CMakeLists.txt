cmake_minimum_required(VERSION 3.22)
project(MusicPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

add_executable(music-player
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
)

target_link_libraries(music-player PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
)
