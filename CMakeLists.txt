cmake_minimum_required(VERSION 4.3.4)
project(MusicPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup()

qt_add_executable(music-player
    src/main.cpp
    src/playercontroller.h
    src/playercontroller.cpp
    src/lyricsparser.h
    src/lyricsparser.cpp
    src/lyricsmodel.h
    src/lyricsmodel.cpp
    src/playermodel.h
    src/playermodel.cpp
)

qt_add_qml_module(music-player
    URI MusicPlayer
    VERSION 1.0
    QML_FILES
        qml/Main.qml
    NO_CACHEGEN
    NO_RESOURCE_TARGET_PATH
)

target_link_libraries(music-player PRIVATE
    Qt6::Quick
    Qt6::Multimedia
)

target_include_directories(music-player PRIVATE src)
