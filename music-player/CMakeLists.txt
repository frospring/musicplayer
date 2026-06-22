cmake_minimum_required(VERSION 3.22)
project(MusicPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Multimedia)

qt_add_executable(music-player
    src/main.cpp
    src/appcontext.h          src/appcontext.cpp
    src/audioengine.h         src/audioengine.cpp
    src/audiocontroller.h     src/audiocontroller.cpp
    src/lyricparser.h         src/lyricparser.cpp
    src/lyricmodel.h          src/lyricmodel.cpp
    src/playlistmodel.h       src/playlistmodel.cpp
    src/filemanager.h         src/filemanager.cpp
    resources.qrc
)

target_link_libraries(music-player PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
)
