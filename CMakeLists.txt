cmake_minimum_required(VERSION 3.10)
project(MAIN)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CAMKE_BUILD_TYPE Release)
ENDIF()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(src DIR_SRCS)
set(SRC_PATH ${DIR_SRCS})

include_directories(
${PROJECT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME} ${SRC_PATH})