all: lab1-1

lab1-1: lab1-1.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Mac/MicroGlut.m
	g++ -Wall -o lab1-1 -DGL_GLEXT_PROTOTYPES \
	    lab1-1.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Mac/MicroGlut.m \
	    -I../common -I../common/Mac \
	    -lm -framework OpenGL -framework Cocoa

clean:
	rm -f lab1-1
