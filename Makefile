INCLUDES = -I$(PANDORA_DIR)/PandoraSDK/include

CC = g++
CFLAGS = -c -Wall -g -w -fPIC -O2
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES  = $(wildcard $(PANDORA_DIR)/PandoraSDK/src/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Api/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Helpers/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Managers/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Objects/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Pandora/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Persistency/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/PandoraSDK/src/Xml/*.cc)

OBJECTS = $(SOURCES:.cc=.o)
DEPENDS = $(OBJECTS:.o=.d)

LIBS =

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PANDORA_DIR)/lib/libPandoraSDK.so

all: $(SOURCES) $(OBJECTS)
	@echo $(PROJECT_SOURCE_DIR)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

-include $(DEPENDS)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -MP -MMD -MT $*.o -MT $*.d -MF $*.d -o $*.o $*.cc

install:
ifdef INCLUDE_TARGET
	rsync -r --exclude=.svn $(PANDORA_DIR)/PandoraSDK/include/ ${INCLUDE_TARGET}
endif
ifdef LIB_TARGET
	cp $(PANDORA_DIR)/lib/libPandoraSDK.so ${LIB_TARGET}
endif

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(LIBRARY)
