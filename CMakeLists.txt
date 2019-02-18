cmake_minimum_required (VERSION 3.10 FATAL_ERROR)

project (isomesh_viewer LANGUAGES CXX)

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 17)

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)

find_package (Qt5Widgets CONFIG REQUIRED)
find_package (glm CONFIG REQUIRED)
find_package (isomesh CONFIG REQUIRED)

set (ISOMESH_VIEWER_HEADERS
	src/camera.h
	src/functionselector.h
	src/mainwindow.h
	src/meshgenerator.h
	src/viewerwidget.h
)

set (ISOMESH_VIEWER_SOURCES
	src/camera.cpp
	src/functionselector.cpp
	src/main.cpp
	src/mainwindow.cpp
	src/meshgenerator.cpp
	src/viewerwidget.cpp
)

source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX Headers FILES ${ISOMESH_VIEWER_HEADERS})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX Sources FILES ${ISOMESH_VIEWER_SOURCES})

add_executable (isomesh_viewer
	${ISOMESH_VIEWER_HEADERS}
	${ISOMESH_VIEWER_SOURCES}
	src/mainwindow.ui
	src/resources.qrc
)

target_link_libraries (isomesh_viewer glm isomesh Qt5::Widgets)
