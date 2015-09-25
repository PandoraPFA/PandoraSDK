# cmake file for building PandoraSDK
#-------------------------------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "PandoraSDK requires an out-of-source build.")
endif()

project(PandoraSDK)

set(${PROJECT_NAME}_VERSION_MAJOR 02)
set(${PROJECT_NAME}_VERSION_MINOR 00)
set(${PROJECT_NAME}_VERSION_PATCH 02)
set(${PROJECT_NAME}_VERSION "${PandoraSDK_VERSION_MAJOR}.${PandoraSDK_VERSION_MINOR}.${PandoraSDK_VERSION_PATCH}")

#-------------------------------------------------------------------------------------------------------------------------------------------
# Dependencies
include(PandoraCMakeSettings)

#-------------------------------------------------------------------------------------------------------------------------------------------
# Low level settings - compiler etc
if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "-std=c++11")
    set(PROCEED_WITHOUT_CXX_FLAGS ON)
endif()

include(CheckCXXCompilerFlag)
unset(COMPILER_SUPPORTS_CXX_FLAGS CACHE)
CHECK_CXX_COMPILER_FLAG(${CMAKE_CXX_FLAGS} COMPILER_SUPPORTS_CXX_FLAGS)

if(NOT COMPILER_SUPPORTS_CXX_FLAGS)
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} does not support cxx flags ${CMAKE_CXX_FLAGS}")
    if(NOT PROCEED_WITHOUT_CXX_FLAGS)
        message(FATAL_ERROR "CMake abort.")
    endif()
    message(STATUS "Proceed without cxx flags ${CMAKE_CXX_FLAGS}")
    unset(CMAKE_CXX_FLAGS)
endif()

if(APPLE)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic -Wno-long-long -Wno-sign-compare -Wshadow -fno-strict-aliasing ${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ansi -pedantic -Wno-long-long -Wno-sign-compare -Wshadow -fno-strict-aliasing ${CMAKE_CXX_FLAGS}")
endif()

#-------------------------------------------------------------------------------------------------------------------------------------------
# Build products

#- include directories
include_directories(include)

# - Collect sources - not ideal because you have to keep running CMake to pick up changes
file(GLOB_RECURSE PANDORA_SDK_SRCS RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cc")

# - Add library and properties
add_library(${PROJECT_NAME} SHARED ${PANDORA_SDK_SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT_NAME}_VERSION} SOVERSION ${${PROJECT_NAME}_SOVERSION})

# - Optional documents
option(PandoraSDK_BUILD_DOCS "Build documentation for ${PROJECT_NAME}" OFF)
if(PandoraSDK_BUILD_DOCS)
    add_subdirectory(doc)
endif()

#-------------------------------------------------------------------------------------------------------------------------------------------
# Install products

# - library
install(TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT Runtime)

# - headers
install(DIRECTORY include/ DESTINATION include COMPONENT Development FILES_MATCHING PATTERN "*.h")

# - support files
PANDORA_GENERATE_PACKAGE_CONFIGURATION_FILES(${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake)

#-------------------------------------------------------------------------------------------------------------------------------------------
# display some variables and write them to cache
PANDORA_DISPLAY_STD_VARIABLES()
