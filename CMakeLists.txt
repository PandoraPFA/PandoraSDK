########################################################
# cmake file for building FineGranularityContent
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( FineGranularityContent )


# project version
SET( FineGranularityContent_VERSION_MAJOR 0 )
SET( FineGranularityContent_VERSION_MINOR 16 )
SET( FineGranularityContent_VERSION_PATCH 0 )


# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )


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

ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
ADD_DEFINITIONS( "-Wno-long-long" )

AUX_SOURCE_DIRECTORY( ./src/ FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCCheating FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCClustering FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCFragmentRemoval FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCHelpers FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCMonitoring FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCParticleId FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCPfoConstruction FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCPlugins FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCReclustering FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCTopologicalAssociation FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCTrackClusterAssociation FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/LCUtility FineGranularityContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )


### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

FIND_PACKAGE( PandoraSDK 0.16 REQUIRED )
FOREACH( pkg PandoraSDK )
    IF( ${pkg}_FOUND )
        INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()

IF( PANDORA_MONITORING )
    FIND_PACKAGE( PandoraMonitoring 0.16 REQUIRED )
    IF( PandoraMonitoring_FOUND )
        INCLUDE_DIRECTORIES( ${PandoraMonitoring_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${PandoraMonitoring_LIBRARIES} )
        ADD_DEFINITIONS ( ${PandoraMonitoring_DEFINITIONS} )
        ADD_DEFINITIONS( "-DMONITORING" )
    ENDIF()
ENDIF()


### LIBRARY #################################################################

# add library
ADD_SHARED_LIBRARY( FineGranularityContent ${FineGranularityContent_SRCS} )

# install library
INSTALL_SHARED_LIBRARY( FineGranularityContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install cmake configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( FineGranularityContentConfig.cmake FineGranularityContentConfigVersion.cmake FineGranularityContentLibDeps.cmake )
