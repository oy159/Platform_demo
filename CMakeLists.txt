cmake_minimum_required(VERSION 3.25)
project(platform_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "E:/Program_Files/QT6/6.7.1/mingw_64/lib/cmake")

find_package(Qt6 COMPONENTS
        Core
        Widgets
        Network
        Charts
        REQUIRED)
file(GLOB_RECURSE SOURCES "ui/*.cpp" "thread/*.cpp" "FFT/*.cpp")
include_directories(ui thread FFT .)

add_executable(platform_demo main.cpp ${SOURCES})
target_link_libraries(platform_demo
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Charts
        )

set(FFTW_DIR "D:/Program_Data/Clion/Cpp/QT/platform_demo/FFT")
link_directories(${FFTW_DIR})
target_link_libraries(platform_demo
        ${FFTW_DIR}/libfftw3-3.lib
        ${FFTW_DIR}/libfftw3f-3.lib
        ${FFTW_DIR}/libfftw3l-3.lib
        )


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

find_library(VISA_LIBRARY
        NAMES visa32 visa64
        PATHS "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Lib_x64/msc")
INCLUDE_DIRECTORIES("C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include")
if (VISA_LIBRARY)
    target_link_libraries(platform_demo ${VISA_LIBRARY})
endif ()
