// Lab 1-1, multi-pass rendering with FBOs and HDR.
// Revision for 2013, simpler light, start with rendering on quad in final stage.
// Switched to low-res Stanford Bunny for more details.
// No HDR is implemented to begin with. That is your task.

// 2018: No zpr, trackball code added in the main program.
// 2021: Updated to use LittleOBJLoader.
// 2022: Cleaned up. Made C++ variant.

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "MicroGlut.h"
#define MAIN
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "GL_utilities.h"
// uses framework Cocoa
// uses framework OpenGL

// Window size is defined here since it is also used for the FBOs
const int initWidth=800, initHeight=800;

#define NUM_LIGHTS 4

mat4 projectionMatrix;
mat4 viewMatrix, modelToWorldMatrix;


GLfloat square[] = {
							-1,-1,0,
							-1,1, 0,
							1,1, 0,
							1,-1, 0};
GLfloat squareTexCoord[] = {
							 0, 0,
							 0, 1,
							 1, 1,
							 1, 0};
GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

Model* squareModel;

//----------------------Globals-------------------------------------------------
Model *model1;
FBOstruct *fbo1, *fbo2, *fbo3, *fbo4, *fbo5, *fbo6;
GLuint phongshader = 0, plaintextureshader = 0, lp = 0, lpx = 0, lpy = 0, threshold = 0, add = 0;

//-------------------------------------------------------------------------------------

void init(void)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	// Load and compile shaders
	plaintextureshader = loadShaders("plaintextureshader.vert", "plaintextureshader.frag");  // puts texture on teapot
	phongshader = loadShaders("phong.vert", "phong.frag");  // renders with light (used for initial renderin of teapot)
	lp = loadShaders("lp.vert", "lp.frag");
	lpx = loadShaders("lpx.vert", "lpx.frag");
	lpy = loadShaders("lpy.vert", "lpy.frag");
	threshold = loadShaders("threshold.vert", "threshold.frag");
	add = loadShaders("add.vert", "add.frag");

	printError("init shader");

	fbo1 = initFBO(initWidth, initHeight, 0);
	fbo2 = initFBO(initWidth, initHeight, 0);
	fbo3 = initFBO(initWidth, initHeight, 0);
	fbo4 = initFBO(initWidth, initHeight, 0);
	fbo5 = initFBO(initWidth, initHeight, 0);
	fbo6 = initFBO(initWidth, initHeight, 0);

	// load the model
	model1 = LoadModel("stanford-bunny.obj");

	squareModel = LoadDataToModel(
			(vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL,
			squareIndices, 4, 6);

	vec3 cam = vec3(0, 5, 15);
	vec3 point = vec3(0, 1, 0);
	vec3 up = vec3(0, 1, 0);
	viewMatrix = lookAtv(cam, point, up);
	modelToWorldMatrix = IdentityMatrix();
}

// Run a filter shader on the input texture(s) and render to the output FBO
void runfilter(GLuint shader, FBOstruct *in1, FBOstruct *in2, FBOstruct *out)
{
    glUseProgram(shader);

    // Many of these things would be more efficiently done once and for all
	glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glUniform1i(glGetUniformLocation(shader, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(shader, "texUnit2"), 1);
    useFBO(out, in1, in2);

    DrawModel(squareModel, shader, "in_Position", NULL, "in_TexCoord");

    glFlush();
}

static void extractBrightAreas(void)
{
    glUseProgram(threshold);
    glUniform1f(glGetUniformLocation(threshold, "threshold"), 1.0f);
    runfilter(threshold, fbo1, 0L, fbo2);
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
    mat4 vm2;

    // 1. Rendera kaninen till fbo1
    useFBO(fbo1, 0L, 0L);
    glClearColor(0.2, 0.2, 0.5, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(phongshader);
    vm2 = viewMatrix * modelToWorldMatrix;
    vm2 = vm2 * T(0, -8.5, 0);
    vm2 = vm2 * S(80,80,80);

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
    glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    DrawModel(model1, phongshader, "in_Position", "in_Normal", NULL);

    // 2. Horisontellt lågpassfilter (fbo1 -> fbo2)
    glUseProgram(lpx);
    glUniform2f(glGetUniformLocation(lpx, "texelSize"), 1.0f / width, 0.0f);
    runfilter(lpx, fbo1, 0L, fbo2);

	extractBrightAreas();

    // 3. Vertikalt lågpassfilter (fbo2 -> fbo3)
    glUseProgram(lpy);
    glUniform2f(glGetUniformLocation(lpy, "texelSize"), 0.0f, 1.0f / height);
    runfilter(lpy, fbo2, 0L, fbo3);

    // 4. Rita resultatet från fbo3 till skärmen
    useFBO(0L, fbo3, 0L);
    glClearColor(0.0, 0.0, 0.0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// --- 1d: Blooming ------------------------------
	// 1. Threshold pass (fbo3 → fbo4)
	glUseProgram(threshold);
	glUniform1f(glGetUniformLocation(threshold, "threshold"), 1.0f); // justera värdet
	runfilter(threshold, fbo3, 0L, fbo4);

	// 2. Blur threshold-bilden (fbo4 → fbo5 → fbo6)
	glUseProgram(lpx);
	glUniform2f(glGetUniformLocation(lpx, "texelSize"), 1.0f / width, 0.0f);
	runfilter(lpx, fbo4, 0L, fbo5);

	glUseProgram(lpy);
	glUniform2f(glGetUniformLocation(lpy, "texelSize"), 0.0f, 1.0f / height);
	runfilter(lpy, fbo5, 0L, fbo6);

	// 3. Kombinera original (fbo3) + blurred highlights (fbo6) → skärm
	glUseProgram(add); // ny shader för att addera två texturer
	runfilter(add, fbo3, fbo6, 0L);

    // glUseProgram(plaintextureshader);
    // glUniform1i(glGetUniformLocation(plaintextureshader, "texUnit"), 0);
    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);

    DrawModel(squareModel, plaintextureshader, "in_Position", NULL, "in_TexCoord");

    glutSwapBuffers();
}


void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	projectionMatrix = perspective(90, ratio, 1.0, 1000);
}

// Trackball

int prevx = 0, prevy = 0;

void mouseUpDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		prevx = x;
		prevy = y;
	}
}

void mouseDragged(int x, int y)
{
	vec3 p;
	mat4 m;
	
	// This is a simple and IMHO really nice trackball system:
	
	// Use the movement direction to create an orthogonal rotation axis

	p.y = x - prevx;
	p.x = -(prevy - y);
	p.z = 0;

	// Create a rotation around this axis and premultiply it on the model-to-world matrix
	// Limited to fixed camera! Will be wrong if the camera is moved!

	m = ArbRotate(p, sqrt(p.x*p.x + p.y*p.y) / 50.0); // Rotation in view coordinates	
	modelToWorldMatrix = Mult(m, modelToWorldMatrix);
	
	prevx = x;
	prevy = y;
	
	glutPostRedisplay();
}


//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(initWidth, initHeight);

	glutInitContextVersion(3, 2);
	glutCreateWindow ("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseUpDown);
	glutMotionFunc(mouseDragged);
	glutRepeatingTimer(50);

	init();
	glutMainLoop();
	exit(0);
}

