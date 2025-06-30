cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Enforce out-of-source builds.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "PandoraSDK requires an out-of-source build. Please create a separate build directory.")
endif()

project(PandoraSDK VERSION 4.0.2 LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install path for PandoraSDK" FORCE)
endif()

# -------------------------------------------------------------------------------------------------------------------------------------------
# Create the PandoraSDK library target

# Include the list of source files from a separate file to keep this one clean.
include(cmake/PandoraSDK_sources.cmake)

add_library(${PROJECT_NAME} SHARED ${PANDORA_SDK_SRCS})
add_library(PandoraPFA::PandoraSDK ALIAS ${PROJECT_NAME})

# Set library versioning properties.
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Target properties (include directories, compile features, etc.)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
    -Wno-long-long
    -Wno-sign-compare
    -Wshadow
    -fno-strict-aliasing
)

# Optional documentation
option(PandoraSDK_BUILD_DOCS "Build documentation for ${PROJECT_NAME}" OFF)
if(PandoraSDK_BUILD_DOCS)
    add_subdirectory(doc)
endif()

# Installation

# Install the library target.
install(TARGETS ${PROJECT_NAME}
    EXPORT PandoraSDKTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
)

# Install the public header files.
install(DIRECTORY include/
    DESTINATION include
    COMPONENT Development
)

# Include standard CMake modules for generating package configuration files.
include(CMakePackageConfigHelpers)

# Generate the PandoraSDKConfigVersion.cmake file.
# This file is used by find_package to check version compatibility.
write_basic_package_version_file(
    "PandoraSDKConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Configure the PandoraSDKConfig.cmake.in file.
# This creates the main package configuration file.
configure_package_config_file(
    "cmake/PandoraSDKConfig.cmake.in"
    "PandoraSDKConfig.cmake"
    INSTALL_DESTINATION lib/cmake/PandoraSDK
)

# Install the generated package configuration files.
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PandoraSDKConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PandoraSDKConfigVersion.cmake"
    DESTINATION lib/cmake/PandoraSDK
    COMPONENT Development
)

# Install the export file that contains information about the installed targets.
install(EXPORT PandoraSDKTargets
    FILE PandoraSDKTargets.cmake
    NAMESPACE PandoraPFA::
    DESTINATION lib/cmake/PandoraSDK
    COMPONENT Development
)
