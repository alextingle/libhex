CXXFLAGS += -Wall

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

.PHONY: all
all: test

deps.mk: $(wildcard *.cc) $(wildcard *.h)
	g++ -MM $(wildcard *.cc) > $@

include deps.mk

test: test.o $(patsubst %.cc,%.o,$(SOURCES))
	g++ $(LDFLAGS) $^ $(LIBS) -o $@

.PHONY: clean
clean:
	rm -f *.o test
