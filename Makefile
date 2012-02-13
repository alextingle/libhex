#                            Package   : libhex
# Makefile                   Created   : 2007/10/13
#                            Author    : Alex Tingle
#
#    Copyright (C) 2007-2008, Alex Tingle.
#
#    This file is part of the libhex application.
#
#    libhex is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    libhex is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

CPPFLAGS += \
 -I$(PYINC) \

CXXFLAGS += \
 -Wall \
 -fno-strict-aliasing \
 -fPIC \

DEBUG := 1

VERSION_MAJOR := 0
VERSION_MINOR := 1

OBJDIR := $(BUILDDIR)OBJ

CCFILES := \
 algorithm.cc \
 area.cc \
 boundary.cc \
 boundingbox.cc \
 direction.cc \
 edge.cc \
 grid.cc \
 hex.cc \
 move.cc \
 path.cc \
 svg.cc \

OFILES   := $(addprefix $(OBJDIR)/,$(CCFILES:.cc=.o))
DEPFILES := $(addprefix $(OBJDIR)/,$(CCFILES:.cc=.dep))

OBJDIRS := $(sort $(dir $(OFILES) $(DEPFILES)))

ifeq ($(DEBUG),1)
  CXXFLAGS += -g
else
  CXXFLAGS += -O2
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
all: lib solib pylib test README

$(OBJDIR)/%.dep: %.cc | $(dir $(OBJDIR)/%)
	g++ -MM $(CPPFLAGS) $< -o $@ -MT $@

$(OBJDIR)/%.o: %.cc $(OBJDIR)/%.dep
	g++ -c  $(CPPFLAGS) $(CXXFLAGS) $< -o $@

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPFILES)
endif

# Make object directories
$(OBJDIRS):
	mkdir -p $@

hex_wrap.cc: hex.i $(wildcard *.h)
	swig -o $@ -classic -c++ -python $<

.PHONY: lib
lib: libhex.a
libhex.a: $(OFILES)
	ar rus $@ $^

.PHONY: solib
solib: libhex$(SOEXT)
libhex$(SOEXT): $(OFILES)
	$(call MakeSharedLib,$^,$@)

.PHONY: pylib
pylib: _hex.so
_hex.so: $(OBJDIR)/hex_wrap.o $(OFILES)
	$(call MakeSharedLib,$^ $(LIBS),$@)

test: $(OBJDIR)/test.o libhex$(SOEXT)
	g++ $(LDFLAGS) $< $(LIBS) -o $@ -L. -lhex

.PHONY: install
install: libhex$(SOEXT)
	install libhex$(SOEXT) /usr/local/lib
	install hex.h hexsvg.h hexmove.h /usr/local/include

.PHONY: clean
clean:
	rm -rf $(OBJDIR) *$(SOEXT) *.a test hex_wrap.cc hex.py *.pyc

README: hex.h
	sed -n -e '/\/\*\* *@mainpage/,/\*\//p' $< \
	| sed -e 's%^[/* ]*\(@mainpage \)\?%%' > $@

# Eliminate all default rules.
.SUFFIXES:

# Don't leave half-finished targets lying around.
.DELETE_ON_ERROR:

