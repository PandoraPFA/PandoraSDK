# cmake file for building PandoraSDK
#-------------------------------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "PandoraSDK requires an out-of-source build.")
endif()

# project name/Version
project(PandoraSDK VERSION 3.3.1)

#-------------------------------------------------------------------------------------------------------------------------------------------
# Dependencies
# Standard install directories
include(GNUInstallDirs)
if(NOT CMAKE_INSTALL_CMAKEDIR)
  set(CMAKE_INSTALL_CMAKEDIR "" CACHE PATH "CMake package configuration files (LIBDIR/cmake)")
  set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake")
endif()

#-------------------------------------------------------------------------------------------------------------------------------------------
# Low level settings - compiler etc
# If we're generating for single-mode and no build type has been set,
# default to RelWithDebInfo
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo"
      FORCE
      )
  else()
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo"
      FORCE
      )
  endif()
endif()

# C++ Standard
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ Standard to compile against")

# - GNU/Clang/Intel
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|(Apple)Clang|Intel")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic -Wno-long-long -Wno-sign-compare -Wshadow -fno-strict-aliasing ${CMAKE_CXX_FLAGS}")
endif()

#-------------------------------------------------------------------------------------------------------------------------------------------
# Build products

# - Collect sources - not ideal because you have to keep running CMake to pick up changes
file(GLOB_RECURSE PANDORA_SDK_SRCS RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cc")

# - Add library and usage requirements
add_library(${PROJECT_NAME} SHARED ${PANDORA_SDK_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_MAJOR_VERSION}
    )

# - Optional documents
option(PandoraSDK_BUILD_DOCS "Build documentation for ${PROJECT_NAME}" OFF)
if(PandoraSDK_BUILD_DOCS)
    add_subdirectory(doc)
endif()

#-------------------------------------------------------------------------------------------------------------------------------------------
# Install products
# - library
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime)

# - headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development FILES_MATCHING PATTERN "*.h")

# - support files
# CMake
# Create config files once for build dir and once for install dir.
# This alllows us to use the build as an in-place install identically
# to a "normal" install.
include(CMakePackageConfigHelpers)
# - Common to both trees
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
  )
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}"
  COMPONENT Development)

# - Build tree
# ... config
set(INSTALL_INCLUDEDIR "${PROJECT_SOURCE_DIR}/include")
configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION "${PROJECT_BINARY_DIR}"
  INSTALL_PREFIX "${PROJECT_BINARY_DIR}"
  PATH_VARS INSTALL_INCLUDEDIR
  )
# ... targets
export(EXPORT ${PROJECT_NAME}Targets NAMESPACE ${PROJECT_NAME}:: FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

# - Install tree
# ... config
set(INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/InstallTree/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  PATH_VARS INSTALL_INCLUDEDIR
  )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/InstallTree/${PROJECT_NAME}Config.cmake
  DESTINATION "${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}"
  COMPONENT Development)

# ... targets
install(EXPORT ${PROJECT_NAME}Targets NAMESPACE ${PROJECT_NAME}:: DESTINATION "${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}")


