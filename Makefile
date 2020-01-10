CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-O2 -std=c++11 
LDFLAGS=
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system

SRCS=atoms.cc support.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: atoms

atoms: $(OBJS)
	$(CXX) $(LDFLAGS) -o atoms $(OBJS) $(LDLIBS)

atoms.o: atoms.cc support.hh

support.o: support.hh support.cc

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) atoms
