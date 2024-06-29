cmake_minimum_required(VERSION 3.14)

project(MusicPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifica il percorso di Qt
set(Qt6_DIR "/home/delsa/Qt/6.8.0/gcc_64/lib/cmake/Qt6")

# Trova i componenti di Qt6 richiesti
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

# Aggiungi il percorso personalizzato per FindTaglib.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Trova e aggiungi la libreria taglib
find_package(Taglib REQUIRED)

# Trova FFTW3 usando pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3 REQUIRED fftw3)

# Aggiungi automaticamente l'esecuzione di moc, uic e rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Aggiungi i file sorgente
add_executable(MusicPlayer
    main.cpp
    mainwindow/MainWindow.cpp
    mainwindow/MainWindow.h
    player/MusicPlayer.cpp
    player/MusicPlayer.h
    search/Search.cpp
    search/Search.h
    widgets/MarqueeLabel.cpp
    widgets/MarqueeLabel.h
)

# Aggiungi manualmente i percorsi degli include
target_include_directories(MusicPlayer PRIVATE
    /home/delsa/Qt/6.8.0/gcc_64/include/QtCore
    /home/delsa/Qt/6.8.0/gcc_64/include/QtGui
    /home/delsa/Qt/6.8.0/gcc_64/include/QtWidgets
    /home/delsa/Qt/6.8.0/gcc_64/include/QtMultimedia
    ${CMAKE_SOURCE_DIR}/mainwindow
    ${CMAKE_SOURCE_DIR}/player
    ${CMAKE_SOURCE_DIR}/search
    ${CMAKE_SOURCE_DIR}/widgets
    ${TAGLIB_INCLUDE_DIRS}
    ${FFTW3_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(MusicPlayer
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    ${TAGLIB_LIBRARIES}
    ${FFTW3_LIBRARIES}
    -lz
)

# Controlla che QtMultimedia sia stato trovato
if(NOT Qt6Multimedia_FOUND)
    message(FATAL_ERROR "Qt6Multimedia not found!")
endif()
