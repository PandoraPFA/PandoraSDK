ifndef PROJECT_DIR
    PROJECT_DIR = $(PANDORA_DIR)/LCContent
    PROJECT_LIBRARY_DIR = $(PANDORA_DIR)/lib
else
    PROJECT_LIBRARY_DIR = $(PROJECT_DIR)/lib
endif

ifdef MONITORING
    DEFINES = -DMONITORING=1
endif

INCLUDES  = -I$(PROJECT_DIR)/include
INCLUDES += -I$(PANDORA_DIR)/PandoraSDK/include
ifdef MONITORING
    INCLUDES += -I$(PANDORA_DIR)/PandoraMonitoring/include
endif

CC = g++
CFLAGS = -c -g -fPIC -O2 -Wall -Wextra -pedantic -Wshadow -Werror -ansi
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES  = $(wildcard $(PROJECT_DIR)/src/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCCheating/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCClustering/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCFragmentRemoval/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCHelpers/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCMonitoring/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCParticleId/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCPersistency/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCPfoConstruction/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCPlugins/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCReclustering/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCTopologicalAssociation/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCTrackClusterAssociation/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/LCUtility/*.cc)

OBJECTS = $(SOURCES:.cc=.o)
DEPENDS = $(OBJECTS:.o=.d)

LIBS = -L$(PANDORA_DIR)/lib -lPandoraSDK

ifdef MONITORING
    LIBS += -lPandoraMonitoring
endif

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PROJECT_LIBRARY_DIR)/libLCContent.so

all: $(SOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

-include $(DEPENDS)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -MP -MMD -MT $*.o -MT $*.d -MF $*.d -o $*.o $*.cc

install:
ifdef INCLUDE_TARGET
	rsync -r --exclude=.svn $(PROJECT_DIR)/include/ ${INCLUDE_TARGET}
endif
ifdef LIB_TARGET
	cp $(PROJECT_LIBRARY_DIR)/libLCContent.so ${LIB_TARGET}
endif

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(LIBRARY)
