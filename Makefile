CXX = g++
ARCH = -arch i386 # -arch x86_64
CXXFLAGS = -g -O0 -Wall ${ARCH} -I./include -I/usr/include -L./lib -L/usr/lib -lsfml-graphics -lsfml-window -lsfml-system

default: minsam
	./minsam
