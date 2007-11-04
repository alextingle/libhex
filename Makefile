CXXFLAGS += -Wall -I/usr/include/python2.4 -fno-strict-aliasing

ifeq ($(DEBUG),1)
  CXXFLAGS += -g
else
  CXXFLAGS += -O3
endif

LDFLAGS  += $(CXXFLAGS)

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

.PHONY: all
all: test

deps.mk: $(wildcard *.cc) $(wildcard *.h)
	g++ -MM $(wildcard *.cc) > $@

include deps.mk

hex_wrap.cc: hex.i $(wildcard *.h)
	swig -o $@ -classic -c++ -python $<

test: test.o $(patsubst %.cc,%.o,$(SOURCES))
	g++ $(LDFLAGS) $^ $(LIBS) -o $@

.PHONY: pylib
pylib: _hex.so
_hex.so: hex_wrap.o $(patsubst %.cc,%.o,$(SOURCES))
	g++ -shared $(LDFLAGS) $^ $(LIBS) -o $@

.PHONY: clean
clean:
	rm -f *.o *.so test
