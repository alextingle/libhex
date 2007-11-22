CXXFLAGS += \
 -Wall \
 -I/usr/include/python2.4 \
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

SONAME := libhex.so


.PHONY: all
all: lib solib pylib test

deps.mk: $(wildcard *.cc) $(wildcard *.h)
	g++ -MM $(wildcard *.cc) > $@

include deps.mk

hex_wrap.cc: hex.i $(wildcard *.h)
	swig -o $@ -classic -c++ -python $<

.PHONY: lib
lib: libhex.a
libhex.a: $(patsubst %.cc,%.o,$(SOURCES))
	ar rus $@ $^

.PHONY: solib
solib: libhex.so
libhex.so: $(patsubst %.cc,%.o,$(SOURCES))
	g++ -shared -o $@ $(LDFLAGS) $^

.PHONY: pylib
pylib: _hex.so
_hex.so: hex_wrap.o $(patsubst %.cc,%.o,$(SOURCES))
	g++ -shared $(LDFLAGS) $^ $(LIBS) -o $@

test: test.o solib
	g++ $(LDFLAGS) $< $(LIBS) -o $@ -L. -lhex

.PHONY: clean
clean:
	rm -f *.o *.so *.a test hex_wrap.cc hex.py *.pyc
