INCLUDES = -I$(PANDORA_DIR)/Framework/include

CC = g++
CFLAGS = -c -Wall -g -w -fPIC -O2
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES  = $(wildcard $(PANDORA_DIR)/Framework/src/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Api/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Helpers/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Managers/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Objects/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Pandora/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Persistency/*.cc)
SOURCES += $(wildcard $(PANDORA_DIR)/Framework/src/Xml/*.cc)

OBJECTS = $(SOURCES:.cc=.o)
DEPENDS = $(OBJECTS:.o=.d)

LIBS =

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PANDORA_DIR)/lib/libPandoraFramework.so

all: $(SOURCES) $(OBJECTS)
	@echo $(PROJECT_SOURCE_DIR)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

-include $(DEPENDS)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -MP -MMD -MT $*.o -MT $*.d -MF $*.d -o $*.o $*.cc

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(LIBRARY)
