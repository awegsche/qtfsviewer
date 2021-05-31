cmake_minimum_required(VERSION 3.5)

project(qtfsviewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DQCUSTOMPLOT_USE_OPENGL)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets PrintSupport OpenGL REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES})

set(PROJECT_SOURCES
    src/darkstyle.cpp
    src/main.cpp
    src/qcustomplot.cpp
    src/qfilterworker.cpp
    src/tfsdatafiltermodel.cpp
    src/tfsmodel.cpp
    src/tfspropertymodel.cpp
    src/viewer.cpp
    src/viewer.ui
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtfsviewer
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(qtfsviewer SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qtfsviewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(qtfsviewer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::OpenGL
    ${LIBRARIES}
)
