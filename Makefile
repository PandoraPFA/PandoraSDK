CC = g++
CFLAGS = -c -g -fPIC -O2 -Wall -Wextra -pedantic -Wshadow -Werror -ansi -std=c++11
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

LIBS =
ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

PROJECT_INCLUDE_DIR = $(PANDORA_DIR)/PandoraSDK/include/
PROJECT_LIBRARY = $(PANDORA_DIR)/lib/libPandoraSDK.so

INCLUDES = -I$(PROJECT_INCLUDE_DIR)

SOURCES  = $(wildcard $(PANDORA_DIR)/PandoraSDK/src/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Api/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Helpers/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Managers/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Objects/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Pandora/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Persistency/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Plugins/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Xml/*.cc)
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
