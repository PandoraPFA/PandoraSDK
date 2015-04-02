########################################################
# cmake file for building LCContent
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( LCContent )


# project version
SET( LCContent_VERSION_MAJOR 01 )
SET( LCContent_VERSION_MINOR 00 )
SET( LCContent_VERSION_PATCH 01 )


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

AUX_SOURCE_DIRECTORY( ./src/ LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCCheating LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCClustering LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCFragmentRemoval LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCHelpers LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCMonitoring LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCParticleId LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCPfoConstruction LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCPlugins LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCReclustering LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCTopologicalAssociation LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCTrackClusterAssociation LCContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCUtility LCContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

INCLUDE( CheckCXXCompilerFlag )
IF( NOT CMAKE_CXX_FLAGS )
    SET( CMAKE_CXX_FLAGS "-std=c++11" )
    SET( PROCEED_WITHOUT_CXX_FLAGS ON )
ENDIF()
UNSET( COMPILER_SUPPORTS_FLAGS CACHE )
CHECK_CXX_COMPILER_FLAG( ${CMAKE_CXX_FLAGS} COMPILER_SUPPORTS_FLAGS )
IF( COMPILER_SUPPORTS_FLAGS )
    MESSAGE( STATUS "The compiler ${CMAKE_CXX_COMPILER} supports user-provided flags ${CMAKE_CXX_FLAGS}" )
    AUX_SOURCE_DIRECTORY( ./src/LCContentFast LCContent_SRCS )
    ADD_DEFINITIONS( ${CMAKE_CXX_FLAGS} )
ELSE()
    MESSAGE( STATUS "The compiler ${CMAKE_CXX_COMPILER} does not support user-provided flags ${CMAKE_CXX_FLAGS}" )
    IF( NOT PROCEED_WITHOUT_CXX_FLAGS )
        ERROR( "CMake abort." )
    ENDIF()
ENDIF()


### DEPENDENCIES ############################################################

FIND_PACKAGE( PandoraSDK 01.00.00 REQUIRED )
FOREACH( pkg PandoraSDK )
    IF( ${pkg}_FOUND )
        INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()

IF( PANDORA_MONITORING )
    FIND_PACKAGE( PandoraMonitoring 01.00.00 REQUIRED )
    IF( PandoraMonitoring_FOUND )
        INCLUDE_DIRECTORIES( ${PandoraMonitoring_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${PandoraMonitoring_LIBRARIES} )
        ADD_DEFINITIONS ( ${PandoraMonitoring_DEFINITIONS} )
        ADD_DEFINITIONS( "-DMONITORING" )
    ENDIF()
ENDIF()


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
ADD_SHARED_LIBRARY( LCContent ${LCContent_SRCS} )


### INSTALL #################################################################

# install library
INSTALL_SHARED_LIBRARY( LCContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install cmake configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( LCContentConfig.cmake LCContentConfigVersion.cmake LCContentLibDeps.cmake )
