cmake_minimum_required(VERSION 3.5.1)
project(ProgramTracer)

set(CMAKE_CXX_STANDARD 11)

#add_executable(ProgramTracer src/StackTracerManagement.cpp include/StackTracerManagement.h src/MemoryAllocationWrap.cpp include/MemoryAllocationWrap.h)
#
#include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
#
#link_directories(${CMAKE_CURRENT_LIST_DIR}/src)
#file(GLOB SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
#
#add_executable(${PROJECT_NAME} test/main.cpp ${SOURCES} src/TracerSignal.cpp test/signal_test.cpp)
#
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-g -rdynamic -pthread -Wall -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=_Znwm -Wl,--wrap=_ZdlPv")
#

aux_source_directory(./src SRC_FILES)

file(GLOB_RECURSE project_HEADERS STATIC
        "./include/FileManagement.h"
        "./include/MemoryAllocationWrap.h"
        "./include/StackTracerManagement.h"
        "./include/TracerSignal.h")
SET(CMAKE_CXX_FLAGS "-g")
# The version number
set(PATH \"leakInfo\")
set(DEBUG_BUILD true)

configure_file(
        "${PROJECT_SOURCE_DIR}/tracerConfig.h.in"
        "${PROJECT_BINARY_DIR}/tracerConfig.h"
)
include_directories("${PROJECT_BINARY_DIR}")

# configure a header file to pass some of the CMake settings to
# source code(demo11Config.h.in), the file demoConfig.h does't exists
configure_file(
        "${PROJECT_SOURCE_DIR}/tracerConfig.h.in"
        "${PROJECT_BINARY_DIR}/tracerConfig.h"
)
add_executable(${PROJECT_NAME} ./test/main.cpp ${SRC_FILES} ${project_HEADERS})
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LINK_FLAGS "-rdynamic -pthread -Wall -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=_Znwm -Wl,--wrap=_ZdlPv -Wl,--wrap=fopen -Wl,--wrap=fclose -Wl,--wrap=freopen")

