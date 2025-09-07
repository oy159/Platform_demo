cmake_minimum_required(VERSION 3.25)
project(platform_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "d:/Program_Files/QT6/6.9.0/mingw_64/lib/cmake")

find_package(Qt6 COMPONENTS
        Core
        Widgets
        Network
        Charts
        REQUIRED)
file(GLOB_RECURSE SOURCES "ui/*.cpp" "thread/*.cpp" "FFT/*.cpp" "Instrument/*.cpp" "ui/NavigationChartView.h")
include_directories(ui thread FFT Instrument .)
qt_add_resources(RESOURCE_RCC ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

add_executable(platform_demo main.cpp ${SOURCES} ${RESOURCE_RCC})
target_link_libraries(platform_demo
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Charts
        )

# 添加Visa库头文件路径
include_directories(${CMAKE_SOURCE_DIR}/Visa/include)

# 添加Visa库lib路径
link_directories(${CMAKE_SOURCE_DIR}/Visa/lib)

# 链接visa库（根据你的平台选择visa32或visa64）
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64位
    target_link_libraries(platform_demo ${CMAKE_SOURCE_DIR}/Visa/lib/visa64.lib)
else()
    # 32位
    target_link_libraries(platform_demo visa32)
endif()

set(FFTW_DIR "D:/Program_Data/QT/Platform_demo/FFT")
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
 message(STATUS "这是一条状态消息")
endif ()
