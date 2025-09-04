all: lab0

lab0: lab0.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Mac/MicroGlut.m
	g++ -Wall -o lab0 -DGL_GLEXT_PROTOTYPES \
	    lab0.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Mac/MicroGlut.m \
	    -I../common -I../common/Mac \
	    -lm -framework OpenGL -framework Cocoa

clean:
	rm -f lab0
