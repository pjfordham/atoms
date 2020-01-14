CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-O2 -std=c++11 -g
LDFLAGS=
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system

SRCS=atoms.cc support.cc main_sfml.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: atoms

atoms: $(OBJS)
	$(CXX) $(LDFLAGS) -o atoms $(OBJS) $(LDLIBS)

atoms.o: atoms.cc atoms.hh

support.o: support.hh support.cc

main_sfml.o: main_sfml.cc atoms.hh support.hh

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) atoms
