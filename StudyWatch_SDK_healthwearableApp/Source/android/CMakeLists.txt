cmake_minimum_required(VERSION 3.4.1)  # for example

set(PROJECT_BASE_NAME "gen4sdk")

project (${PROJECT_BASE_NAME})

set(INC_DIR "../sdk/inc")

set(FLUTTER_INC_DIR "../flutterClasses/header")

set(CPP_SOURCE_DIR "../sdk/src")

set(FLUTTER_CPP_SOURCE_DIR "../flutterClasses/source")

file(GLOB CPP_SOURCES "${CPP_SOURCE_DIR}/*" "${FLUTTER_CPP_SOURCE_DIR}/*")

#file(GLOB FLUTTER_CPP_SOURCES "${FLUTTER_CPP_SOURCE_DIR}/*")

include_directories(${INC_DIR})

include_directories(${FLUTTER_INC_DIR})

add_library( ${PROJECT_BASE_NAME}

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${CPP_SOURCES})