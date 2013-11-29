CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)

PROJECT(ramen)

SET(ROOT_PATH	 ${PROJECT_SOURCE_DIR}/../..)
SET(SRC_PATH	 ${ROOT_PATH}/src)
SET(INCLUDE_PATH ${ROOT_PATH}/include)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x") 

SET(EXE_SRC	${SRC_PATH}/core.cpp
		${SRC_PATH}/database.cpp
		${SRC_PATH}/main.cpp
		${SRC_PATH}/log.cpp
		${SRC_PATH}/settings.cpp
		${SRC_PATH}/builder/builder.cpp
		${SRC_PATH}/builder/meshbuilder.cpp
		${SRC_PATH}/entity/camera.cpp
		${SRC_PATH}/entity/entity.cpp
		${SRC_PATH}/entity/meshrender.cpp
		${SRC_PATH}/entity/positionable.cpp
		${SRC_PATH}/builder/meshbuilder.cpp
		${SRC_PATH}/graphic/color.cpp
		${SRC_PATH}/graphic/font.cpp
		${SRC_PATH}/graphic/graphic.cpp
		${SRC_PATH}/graphic/material.cpp
		${SRC_PATH}/graphic/mesh.cpp
		${SRC_PATH}/graphic/shader.cpp
		${SRC_PATH}/graphic/graphicUtility.cpp
		${SRC_PATH}/io/entityManipulator.cpp
		${SRC_PATH}/io/fbx.cpp
		${SRC_PATH}/io/fbxUtility.cpp
		${SRC_PATH}/io/filesystem.cpp
		${SRC_PATH}/io/ioUtility.cpp
		${SRC_PATH}/perfmon/profiler.cpp
		${SRC_PATH}/perfmon/resmon.cpp
		)

SET(EXECUTABLE_NAME ramen)

ADD_EXECUTABLE(${EXECUTABLE_NAME} ${EXE_SRC})

FIND_PACKAGE(Boost 1.54.0 COMPONENTS chrono filesystem log program_options system thread REQUIRED)
IF (Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
  TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${Boost_LIBRARIES})
ENDIF(Boost_FOUND)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${SDL2_LIBRARIES})

PKG_SEARCH_MODULE(GLESv2 REQUIRED glesv2)
INCLUDE_DIRECTORIES(${GLESv2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${GLESv2_LIBRARIES})

PKG_SEARCH_MODULE(Freetype REQUIRED freetype2)
INCLUDE_DIRECTORIES(${Freetype_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${Freetype_LIBRARIES})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/module)
FIND_PACKAGE(GLM REQUIRED)
IF (GLM_FOUND)
  INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${GLM_LIBRARIES})
ENDIF(GLM_FOUND)

FIND_PACKAGE(FBX REQUIRED)
IF (FBX_FOUND)
  INCLUDE_DIRECTORIES(${FBX_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${FBX_LIBRARY})
ENDIF(FBX_FOUND)

