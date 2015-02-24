ifndef PROJECT_DIR
    PROJECT_DIR = $(PANDORA_DIR)/PandoraSDK
    PROJECT_LIBRARY_DIR = $(PANDORA_DIR)/lib
else
    PROJECT_LIBRARY_DIR = $(PROJECT_DIR)/lib
endif

CC = g++
CFLAGS = -c -g -fPIC -O2 -Wall -Wextra -pedantic -Wshadow -Werror -ansi -std=c++11
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

LIBS =
ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

PROJECT_INCLUDE_DIR = $(PROJECT_DIR)/include/
PROJECT_LIBRARY = $(PROJECT_LIBRARY_DIR)/libPandoraSDK.so

INCLUDES = -I$(PROJECT_INCLUDE_DIR)

SOURCES  = $(wildcard $(PROJECT_DIR)/src/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Api/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Helpers/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Managers/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Objects/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Pandora/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Persistency/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Plugins/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Xml/*.cc)
OBJECTS = $(SOURCES:.cc=.o)
DEPENDS = $(OBJECTS:.o=.d)

all: library

library: $(SOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(PROJECT_LIBRARY)

-include $(DEPENDS)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -MP -MMD -MT $*.o -MT $*.d -MF $*.d -o $*.o $*.cc

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(PROJECT_LIBRARY)

install:
ifdef INCLUDE_TARGET
	rsync -r --exclude=.svn $(PROJECT_INCLUDE_DIR) ${INCLUDE_TARGET}
endif
ifdef LIB_TARGET
	cp $(PROJECT_LIBRARY) ${LIB_TARGET}
endif
