// Revised 2019 with a bit better variable names.
// Experimental C++ version 2022. Almost no code changes.

#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
// uses framework OpenGL
// uses framework Cocoa

// Model-to-world matrix
// Modify this matrix.
// See below for how it is applied to your model.

//Ändra placering på model
mat4 modelToWorld = mat4( 1.0, 0.0, 0.0, 0.1,
                              0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0,
                              0.0, 0.0, 0.0, 1.0);

mat4 modelToWorld_bunny;
mat4 modelToWorld_teddy;
// World-to-view matrix. Usually set by lookAt() or similar.
mat4 worldToView;
// Projection matrix, set by a call to perspective().
mat4 projectionMatrix;

// Globals
// * Model(s)
Model *bunny;
Model *teddy;
// * Reference(s) to shader program(s)
GLuint program;
// * Texture(s)
GLuint texture;

void init(void)
{
	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	printError("GL inits"); // This is merely a vague indication of where something might be wrong

	projectionMatrix = perspective(90, 1.0, 0.1, 1000);
	worldToView = lookAt(0,0,1.5, 0,0,0, 0,1,0);

	// Load and compile shader
	program = loadShaders("lab0.vert", "lab0.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
	bunny = LoadModel("objects/stanford-bunny.obj");
	printError("load models - bunny");

	teddy = LoadModel("objects/teddy.obj");
	printError("load models - teddy");

	// Load textures
	LoadTGATextureSimple("textures/maskros512.tga",&texture);
	printError("load textures");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// ---- BUNNY ANIMATION -----
	float t = glutGet(GLUT_ELAPSED_TIME)* 0.001f; //Time dependency, seconds
	modelToWorld_bunny = Mult(T(-1.0,0.0,-1.0), Ry(t)); // rotation

	// ----- TEDDY ------
	modelToWorld_teddy = Mult(T(1.0,0.0,-1.0), modelToWorld); // flytta teddy åt sidan
	//modelToWorld_teddy = Mult(Ry(t*0.5), modelToWorld_teddy); // ev rotera långsamt

	// send to shader
	glUniform1f(glGetUniformLocation(program, "uTime"), t);

	// camera-position
	glUniform3f(glGetUniformLocation(program,"viewPos"), 0.0f,0.0f,1.5f);
	glUniform3f(glGetUniformLocation(program,"ambientColor"), 0.1f,0.1f,0.1f);
	glUniform1f(glGetUniformLocation(program,"shininess"), 50.0f);

	// light sources
	int numLights = 3;
	glUniform1i(glGetUniformLocation(program,"numLights"), numLights);
	glUniform3f(glGetUniformLocation(program,"lightPos[0]"), 1.0f,1.0f,1.0f);
	glUniform3f(glGetUniformLocation(program,"lightColor[0]"), 0.0f,0.8f,0.0f);
	glUniform3f(glGetUniformLocation(program,"lightPos[1]"), -1.0f,0.5f,1.0f);
	glUniform3f(glGetUniformLocation(program,"lightColor[1]"), 0.8f,0.0f,0.0f);
	glUniform3f(glGetUniformLocation(program,"lightPos[2]"), 0.0f,0.1f,0.5f);
	glUniform3f(glGetUniformLocation(program,"lightColor[2]"), 0.0f,0.0f,1.0f);

	//activate the program, and set its variables
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program,"exampletexture"),0);//the last argument has to be the same as the texture-unit that is to be used
	glActiveTexture(GL_TEXTURE0);//which texture-unit is active
	glBindTexture(GL_TEXTURE_2D, texture);//load the texture to active texture-unit

	
	//Draw bunny
	mat4 modelToWorldToView = worldToView * modelToWorld_bunny; // Combine to one matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldToView"), 1, GL_TRUE, modelToWorldToView.m);
	DrawModel(bunny, program, "in_Position", "in_Normal", NULL);


	//Draw teddy
	mat4 modelToWorldToView_teddy = Mult(worldToView, modelToWorld_teddy);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldToView"), 1, GL_TRUE, modelToWorldToView_teddy.m);
	DrawModel(teddy, program, "in_Position", "in_Normal", NULL);
		
	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(800, 800);
	glutCreateWindow ("Lab 0 - OpenGL Introduction");
	glutDisplayFunc(display); 
	glutRepeatingTimer(20);
	init ();
	glutMainLoop();
	exit(0);
}

