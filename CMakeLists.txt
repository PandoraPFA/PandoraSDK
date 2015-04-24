########################################################
# cmake file for building PandoraSDK
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( PandoraSDK )


# project version
SET( PandoraSDK_VERSION_MAJOR 01 )
SET( PandoraSDK_VERSION_MINOR 00 )
SET( PandoraSDK_VERSION_PATCH 01 )


### CMAKE ###################################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES QUIET )

IF( ILCUTIL_FOUND )
    INCLUDE( ilcsoft_default_settings )
ELSE()
    INCLUDE( Default_settings )
ENDIF()


### SOURCE ##################################################################

#include directories
INCLUDE_DIRECTORIES( ./include )
INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

AUX_SOURCE_DIRECTORY( ./src SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Api SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Helpers SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Managers SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Objects SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Pandora SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Persistency SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Plugins SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Templates SDK_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Xml SDK_SRCS )

IF( NOT CMAKE_CXX_FLAGS )
    SET( CMAKE_CXX_FLAGS "-std=c++11" )
    SET( PROCEED_WITHOUT_CXX_FLAGS ON )
ENDIF()

INCLUDE( CheckCXXCompilerFlag )

UNSET( COMPILER_SUPPORTS_CXX_FLAGS CACHE )
CHECK_CXX_COMPILER_FLAG( ${CMAKE_CXX_FLAGS} COMPILER_SUPPORTS_CXX_FLAGS )
IF( NOT COMPILER_SUPPORTS_CXX_FLAGS )
    MESSAGE( STATUS "The compiler ${CMAKE_CXX_COMPILER} does not support cxx flags ${CMAKE_CXX_FLAGS}" )
    IF( NOT PROCEED_WITHOUT_CXX_FLAGS )
        MESSAGE( FATAL_ERROR "CMake abort." )
    ENDIF()
    MESSAGE( STATUS "Proceed without cxx flags ${CMAKE_CXX_FLAGS}" )
    UNSET( CMAKE_CXX_FLAGS )
ENDIF()

SET( CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ansi -pedantic -Wno-long-long -Wno-sign-compare -Wshadow -fno-strict-aliasing ${CMAKE_CXX_FLAGS}" )


### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ELSE()
    MESSAGE( STATUS "INSTALL_DOC set to OFF" )
    SET( INSTALL_DOC OFF )
ENDIF()


### LIBRARY #################################################################

# add library
ADD_SHARED_LIBRARY( PandoraSDK ${SDK_SRCS} )


### INSTALL #################################################################

# install library
INSTALL_SHARED_LIBRARY( PandoraSDK DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install cmake configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( PandoraSDKConfig.cmake PandoraSDKConfigVersion.cmake PandoraSDKLibDeps.cmake )
