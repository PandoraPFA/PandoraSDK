########################################################
# cmake file for building Pandora Framework
########################################################

INCLUDE_DIRECTORIES( ./include )

AUX_SOURCE_DIRECTORY( ./src Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Api Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Helpers Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Managers Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Objects Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Pandora Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Persistency Framework_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Xml Framework_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

# add library
ADD_SHARED_LIBRARY( PandoraFramework ${Framework_SRCS} )

# install library
INSTALL_SHARED_LIBRARY( PandoraFramework DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )
