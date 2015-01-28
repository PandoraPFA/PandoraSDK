########################################################
# cmake file for building LCContent
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( LCContent )


# project version
SET( LCContent_VERSION_MAJOR 0 )
SET( LCContent_VERSION_MINOR 17 )
SET( LCContent_VERSION_PATCH 0 )


### DEPENDENCIES ############################################################
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES QUIET )

IF( ILCUTIL_FOUND )
    INCLUDE( ilcsoft_default_settings )
ELSE()
    INCLUDE( Default_settings )
ENDIF()

FIND_PACKAGE( PandoraSDK 0.17 REQUIRED )
FOREACH( pkg PandoraSDK )
    IF( ${pkg}_FOUND )
        INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()

IF( PANDORA_MONITORING )
    FIND_PACKAGE( PandoraMonitoring 0.17 REQUIRED )
    IF( PandoraMonitoring_FOUND )
        INCLUDE_DIRECTORIES( ${PandoraMonitoring_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${PandoraMonitoring_LIBRARIES} )
        ADD_DEFINITIONS ( ${PandoraMonitoring_DEFINITIONS} )
        ADD_DEFINITIONS( "-DMONITORING" )
    ENDIF()
ENDIF()


### DOCUMENTATION ############################################################
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ELSE()
    MESSAGE( STATUS "INSTALL_DOC set to OFF" )
    SET( INSTALL_DOC OFF )
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
ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++11" )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    message( STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++11 support." )
    AUX_SOURCE_DIRECTORY( ./src/LCContentFast LCContent_SRCS )
    ADD_DEFINITIONS( "-std=c++11" )
else()
    message( STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Some functionality will be limited." )
endif()

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )


### LIBRARY #################################################################

# add library
ADD_SHARED_LIBRARY( LCContent ${LCContent_SRCS} )

# install library
INSTALL_SHARED_LIBRARY( LCContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install cmake configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( LCContentConfig.cmake LCContentConfigVersion.cmake LCContentLibDeps.cmake )
