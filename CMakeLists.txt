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

set (ISOMESH_VIEWER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set (ISOMESH_VIEWER_HEADERS
	${ISOMESH_VIEWER_SOURCE_DIR}/camera.h
	${ISOMESH_VIEWER_SOURCE_DIR}/functionselector.h
	${ISOMESH_VIEWER_SOURCE_DIR}/mainwindow.h
	${ISOMESH_VIEWER_SOURCE_DIR}/meshgenerator.h
	${ISOMESH_VIEWER_SOURCE_DIR}/viewerwidget.h
)

set (ISOMESH_VIEWER_SOURCES
	${ISOMESH_VIEWER_SOURCE_DIR}/camera.cpp
	${ISOMESH_VIEWER_SOURCE_DIR}/functionselector.cpp
	${ISOMESH_VIEWER_SOURCE_DIR}/main.cpp
	${ISOMESH_VIEWER_SOURCE_DIR}/mainwindow.cpp
	${ISOMESH_VIEWER_SOURCE_DIR}/meshgenerator.cpp
	${ISOMESH_VIEWER_SOURCE_DIR}/viewerwidget.cpp
)

source_group (TREE ${ISOMESH_VIEWER_SOURCE_DIR} PREFIX Headers FILES ${ISOMESH_VIEWER_HEADERS})
source_group (TREE ${ISOMESH_VIEWER_SOURCE_DIR} PREFIX Sources FILES ${ISOMESH_VIEWER_SOURCES})

add_executable (isomesh_viewer
	${ISOMESH_VIEWER_HEADERS}
	${ISOMESH_VIEWER_SOURCES}
	${ISOMESH_VIEWER_SOURCE_DIR}/mainwindow.ui
	${ISOMESH_VIEWER_SOURCE_DIR}/resources.qrc
)

target_link_libraries (isomesh_viewer glm isomesh Qt5::Widgets)
