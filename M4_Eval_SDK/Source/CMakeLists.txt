cmake_minimum_required(VERSION 2.8.9)
##CONFIGURE SETTINGS
#Below are all the configurable variables in this cmake file, mainly to do with directory structure

#RELEASE BUILD OPTION
#in order to build a release command, you must add the RELEASE=ON option to running cmake.
#instead of running "cmake ..", run "cmake -DRELEASE=ON .." this will create a new release_package
#target, which will copy the appropriate files into a consumer ready folder.
option(RELEASE "RELEASE" OFF)


#PROJECT NAMES
set(PROJECT_BASE_NAME "gen3_sdk")
project (${PROJECT_BASE_NAME})

set(EXE_NAME "main")
set(PYTHON_NAME "${PROJECT_BASE_NAME}")
set(C_SHARP_NAME "${PROJECT_BASE_NAME}_cs")

set(GLOBAL_COMPILE_FLAGS -std=c++11; -Wall)#; -pthread)

#GIT
# Extract git versioning information so we can insert it into the source and the docs
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_DESCRIBE --always)

if (WIN32)
  message( STATUS "Detected platform - Windows")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
	set(BIN_PLATFORM_SUBDIR "windows/$(Platform)")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
	set(BIN_PLATFORM_SUBDIR "windows/$(Platform)")
  endif()
elseif (APPLE)
  message( STATUS "Detected platform - macOS")
  set(BIN_PLATFORM_SUBDIR "macOS")
elseif (UNIX)
  message( STATUS "Detected platform - smells like Linux")
  set(BIN_PLATFORM_SUBDIR "linux")
else()
  message( FATAL_ERROR "Your platform isn't recognized! Add it to the CMakeLists file!")
endif()

#OUTPUT DIRS

set(CPP_EXE_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/test/${BIN_PLATFORM_SUBDIR}/cpp")
set(CPP_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_PLATFORM_SUBDIR}/cpp")
set(CS_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_PLATFORM_SUBDIR}/csharp")
set(PYTHON_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_PLATFORM_SUBDIR}/python")
set(CPP_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_PLATFORM_SUBDIR}/test")
set(DOXYGEN_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/doc/doxygen/")

#SOURCE DIRS
set(CPP_SOURCE_DIR "src")

# file(GLOB SOURCE_FILES ${CPP_SOURCE_DIR}*.cpp)
# set_propery(SOURCE ${SOURCE_FILES} PROPERTY COMPILE_FLAGS ${GLOBAL_COMPILE_FLAGS})

#INCLUDE DIRS
set(INC_DIR "inc")


#SWIG
set(SWIG_INTERFACE_DIR "src/swig")
file(GLOB SWIG_INTERFACES "${SWIG_INTERFACE_DIR}/gen3_sdk.i")
file(GLOB SWIG_INTERFACES_CS "${SWIG_INTERFACE_DIR}/gen3_sdk_cs.i")

#Doxygen
set(DOC_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/doc")
set(DOXYGEN_CUSTOM_FMT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/doc/")



###~Below are the actual build commands for CMake, don't change them unless you know what you're doing~###

# Check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    message("Doxygen build starting..")
    set(DOXYGEN_OUTPUT_DIR ${DOXYGEN_BUILD_DIR})
    # Add the doxygen output dir to the `make clean` targets
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${DOXYGEN_OUTPUT_DIR})
    # set input and output files
    set(DOXYGEN_IN "${DOC_BASE_DIR}/src/doxygen_base_cfg.Doxyfile")
    set(DOXYGEN_OUT "${DOXYGEN_BUILD_DIR}sdk.doxyfile")#"${CMAKE_CURRENT_BINARY_DIR}/sdk.Doxyfile")
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # Find all of the doxygen-related .in files, and configure them using cmake defined variables
    file(GLOB_RECURSE DOXYGEN_CFG_IN "${DOC_BASE_DIR}/src/*.in")
    foreach(F_I ${DOXYGEN_CFG_IN})
      # Strip out the file extension
      string(REGEX REPLACE "\\.[^.]*$" "" F_O ${F_I})
      get_filename_component(F_O ${F_O} NAME)
      # Print out the files we found
      message("Found doxygen input file:: ${F_I}")
      message("    Moving it to:: ${DOXYGEN_BUILD_DIR}/${F_O}")
      configure_file("${F_I}" "${DOXYGEN_BUILD_DIR}/${F_O}" @ONLY)
    endforeach()


    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        #DEPENDS gen3_sdk _gen3_sdk
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        #OUTPUT_DIRECTORY ${DOXYGEN_BUILD_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)


# Main C++ build
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/git_version.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/git_version.cpp" @ONLY)
# Build a list of all the .cpp files in the CPP_SOURCES directory
file(GLOB CPP_SOURCES "${CPP_SOURCE_DIR}/*.cpp")
# Create an executable target for the main.cpp binary
add_executable(${EXE_NAME} ${CPP_SOURCES})

target_include_directories(${EXE_NAME} PUBLIC ${INC_DIR})
target_compile_options(${EXE_NAME} PUBLIC ${GLOBAL_COMPILE_FLAGS})

if ( UNIX )
  target_link_libraries(${EXE_NAME} pthread)
endif ( UNIX )

set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CPP_TEST_DIR} PREFIX "" )#${CPP_EXE_OUTDIR}/main )

if ( MSVC )
    ##Override the Release/Debug folders made by Visual Studio
    set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CPP_TEST_DIR} PREFIX "")#${CPP_EXE_OUTDIR}/main )
    set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CPP_TEST_DIR} PREFIX "")#${CPP_EXE_OUTDIR}/main )
endif ( MSVC )

# Create a static lib target for the ${PROJECT_BASE_NAME}
if ( UNIX )
  add_library(${PROJECT_BASE_NAME} SHARED ${CPP_SOURCES})
else ()
  add_library(${PROJECT_BASE_NAME} ${CPP_SOURCES})
endif ( UNIX )
target_include_directories(${PROJECT_BASE_NAME} PUBLIC ${INC_DIR})
# Set the output directory and name of the ${PROJECT_BASE_NAME} static lib binary
# set_target_properties(${PROJECT_BASE_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_SOURCE_DIR}/bin/cpp/${PROJECT_BASE_NAME}" PREFIX "")
target_compile_options(${PROJECT_BASE_NAME} PUBLIC ${GLOBAL_COMPILE_FLAGS})

if ( UNIX )
  set_target_properties(${PROJECT_BASE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CPP_BIN_DIR})
else ()
  set_target_properties(${PROJECT_BASE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CPP_BIN_DIR} PREFIX "")
endif ( UNIX )




if ( MSVC )
    ##Override the Release/Debug folders made by Visual Studio
    set_target_properties(${PROJECT_BASE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CPP_BIN_DIR} PREFIX "")
    set_target_properties(${PROJECT_BASE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CPP_BIN_DIR} PREFIX "")
endif ( MSVC )

#SWIG
find_package(SWIG REQUIRED)
set(SWIG_PYTHON_NAME "_${PYTHON_NAME}") #SWIG adds the _ prefix to the static library, can't change this


# Include custom SWIG cmake commands
include(${SWIG_USE_FILE})

# Specify where CMAKE is going to place the .py and _wrap.cxx files
set(CMAKE_SWIG_OUTDIR ${PYTHON_BIN_DIR})

# SWIG needs the python libraries to build the C++ wrapper against
if(WIN32)
    #this needs to run so windows can detect the correct version of python
    find_package(PythonInterp 3 REQUIRED)
endif(WIN32)

find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
select_library_configurations(${SWIG_PYTHON_NAME})
include_directories(${INC_DIR})

# Smush the .cpp and .i file lists together
set(SWIG_PYTHON_SOURCES ${CPP_SOURCES} ${SWIG_INTERFACES})

# Tell SWIG that the input source files are c++, not c
set_source_files_properties(${SWIG_INTERFACES} PROPERTIES CPLUSPLUS ON)

# Tell swig to generate python bindings
swig_add_library(${PYTHON_NAME} LANGUAGE python SOURCES ${SWIG_PYTHON_SOURCES})
if (MSVC)
  # Have to split up the debug and optimized/release libs for Visual Studio because ????
  list(GET PYTHON_LIBRARIES 1 PYTHON_LIBRARIES_OPTIMIZED)
  list(GET PYTHON_LIBRARIES 3 PYTHON_LIBRARIES_DEBUG)
  target_link_libraries(${SWIG_PYTHON_NAME} optimized "${PYTHON_LIBRARIES_OPTIMIZED}" debug "${PYTHON_LIBRARIES_DEBUG}")

  # Override the Release/Debug folders made by Visual Studio
  set_target_properties(${SWIG_PYTHON_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PYTHON_BIN_DIR} )
  set_target_properties(${SWIG_PYTHON_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PYTHON_BIN_DIR} )
else ()
  # Sane compilers
  target_link_libraries(${SWIG_PYTHON_NAME} "${PYTHON_LIBRARIES}")
  set_target_properties(${SWIG_PYTHON_NAME} PROPERTIES LINK_FLAGS "${PYTHON_LIBRARIES}")
endif()

target_compile_options(${SWIG_PYTHON_NAME} PUBLIC ${GLOBAL_COMPILE_FLAGS})
target_compile_options(${SWIG_PYTHON_NAME} PUBLIC "-DSWIG_PY")
set_target_properties(${SWIG_PYTHON_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYTHON_BIN_DIR} )


if (WIN32)
  # SWIG Csharp
  set(SWIG_SOURCES_CS ${CPP_SOURCES} ${SWIG_INTERFACES_CS})

  set(CMAKE_SWIG_OUTDIR "${CS_BIN_DIR}")

  set_source_files_properties(${SWIG_INTERFACES_CS} PROPERTIES CPLUSPLUS ON)

  swig_add_library(${C_SHARP_NAME} LANGUAGE csharp SOURCES ${SWIG_SOURCES_CS})
  swig_link_libraries(${C_SHARP_NAME} ${CSHARP_LIBRARIES})

  target_compile_options(${C_SHARP_NAME} PUBLIC ${GLOBAL_COMPILE_FLAGS})

  set_target_properties(${C_SHARP_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CS_BIN_DIR})

  if ( MSVC )
      ##Override the Release/Debug folders made by Visual Studio
      set_target_properties(${C_SHARP_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CS_BIN_DIR})
      set_target_properties(${C_SHARP_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CS_BIN_DIR})
  endif ( MSVC )
endif (WIN32)





if( RELEASE )
##Copy appropriate files for the release package
# EXECUTE_PROCESS(
#   COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
#   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#   OUTPUT_VARIABLE "GIT_SHORT_HASH"
#   ERROR_QUIET
#   OUTPUT_STRIP_TRAILING_WHITESPACE)


set(CS_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_PLATFORM_SUBDIR}/csharp")
set(CPP_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_PLATFORM_SUBDIR}/test")
set(DOXYGEN_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/doc/doxygen/")


add_custom_target( release_package ALL
COMMENT "Copy all relevant files into a release package for customers.")

add_custom_command(
  TARGET release_package POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CPP_BIN_DIR}"
          "${CMAKE_CURRENT_SOURCE_DIR}/gen3_sdk-${GIT_DESCRIBE}/bin/${BIN_PLATFORM_SUBDIR}/cpp")

 add_custom_command(
  TARGET release_package POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${PYTHON_BIN_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/gen3_sdk-${GIT_DESCRIBE}/bin/${BIN_PLATFORM_SUBDIR}/python")

add_custom_command(
  TARGET release_package POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/inc/"
          "${CMAKE_CURRENT_SOURCE_DIR}/gen3_sdk-${GIT_DESCRIBE}/inc/")

add_custom_command(
  TARGET release_package POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/"
          "${CMAKE_CURRENT_SOURCE_DIR}/gen3_sdk-${GIT_DESCRIBE}/doc/")


add_custom_command(
  TARGET release_package POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/test/python"
          "${CMAKE_CURRENT_SOURCE_DIR}/gen3_sdk-${GIT_DESCRIBE}/test/python")
endif( RELEASE )
