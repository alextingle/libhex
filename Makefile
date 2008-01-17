CPPFLAGS += \
 -I$(PYINC) \

CXXFLAGS += \
 -Wall \
 -fno-strict-aliasing \
 -fPIC \

DEBUG := 1

VERSION_MAJOR := 0
VERSION_MINOR := 1

SOURCES := \
 algorithm.cc \
 area.cc \
 boundary.cc \
 direction.cc \
 edge.cc \
 grid.cc \
 hex.cc \
 path.cc \
 svg.cc \

ifeq ($(DEBUG),1)
  CXXFLAGS += -g
else
  CXXFLAGS += -O3
endif

LDFLAGS  += $(CXXFLAGS)

# Use python -V to automatically detect the python version
PYVER := $(shell python -V 2>&1 | sed 's/.* \([1-9][.][0-9]\).*/\1/')
PYINC := /usr/include/python$(PYVER)

# Use uname to automatically detect Mac builds.
PLATFORM := $(shell uname -a | sed 's/.*Darwin.*/mac/')

ifeq ($(PLATFORM),mac)
  SOEXT := .dylib
  # Build a shared object library: Use as $(call MakeSharedLib,OBJECTS,SONAME)
  MakeSharedLib = \
    MACOSX_DEPLOYMENT_TARGET=10.3 \
    g++ -dynamiclib -undefined dynamic_lookup $(LDFLAGS) -o $(2) $(1)
else
  SOEXT= .so
  # Build a shared object library: Use as $(call MakeSharedLib,OBJECTS,SONAME)
  MakeSharedLib = g++ -shared $(LDFLAGS) -o $(2) $(1) 
endif


.PHONY: all
all: lib solib pylib test

deps.mk: $(wildcard *.cc) $(wildcard *.h)
	g++ $(CPPFLAGS) -MM $(wildcard *.cc) > $@

include deps.mk

hex_wrap.cc: hex.i $(wildcard *.h)
	swig -o $@ -classic -c++ -python $<

.PHONY: lib
lib: libhex.a
libhex.a: $(patsubst %.cc,%.o,$(SOURCES))
	ar rus $@ $^

.PHONY: solib
solib: libhex$(SOEXT)
libhex$(SOEXT): $(patsubst %.cc,%.o,$(SOURCES))
	$(call MakeSharedLib,$^,$@)

.PHONY: pylib
pylib: _hex.so
_hex.so: hex_wrap.o $(patsubst %.cc,%.o,$(SOURCES))
	$(call MakeSharedLib,$^ $(LIBS),$@)

test: test.o solib
	g++ $(LDFLAGS) $< $(LIBS) -o $@ -L. -lhex

.PHONY: clean
clean:
	rm -f *.o *$(SOEXT) *.a test hex_wrap.cc hex.py *.pyc
