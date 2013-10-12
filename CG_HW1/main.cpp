/*
 * NTUCSIE ICG2010
 * R99922020 Yu-Hsiang Huang
 */

#include <cmath>
#include <iostream>
#include <vector>
using namespace std;
//#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glew.h>
#include <GL/glut.h>
#include "scene.h"
#include "shader.h"
#include "utils.h"

#define MAX_LIGHTS 8

enum ShadingMode {
	FLAT_SHADING = 0,
	GOURAUD_SHADING,
	PHONG_SHADING,
	PHONG_SHADING2
} shadingMode;

enum ControlMode {
	LIGHT0 = GL_LIGHT0,
	LIGHT1,
	LIGHT2,
	LIGHT3,
	LIGHT4,
	LIGHT5,
	LIGHT6,
	LIGHT7,
	MODEL,
	CAMERA
} controlMode;

enum LightControlMode {
	REVOLUTION = LIGHT0 - 5,
	ATTENUATION,
	AMBIENT,
	DIFFUSE,
	SPECULAR
} lightControlMode;

Shader shader, shader2;
Model model;
Camera camera;
Light lights[MAX_LIGHTS];
bool autoRotate, enableLights, printInfo;

void changeShader(ShadingMode s)
{
	switch(s) {
		// Flat shading
		case FLAT_SHADING:
			glShadeModel(GL_FLAT);
			glUseProgram(0); // reset to defualt shaders
			printf("## Flat shading\n");
			break;
		// Gouraud shading
		case GOURAUD_SHADING:
			glShadeModel(GL_SMOOTH);
			glUseProgram(0); // reset to defualt shaders
			printf("## Gouraud shading\n");
			break;
		// Phong shading (Blinn-Phong)
		case PHONG_SHADING:
			shader.useProgram();
			printf("## Phong shading (Blinn-Phong)\n");
			break;
		// Phong shading
		case PHONG_SHADING2:
			shader2.useProgram();
			printf("## Phong shading\n");
			break;
		
	}
	shadingMode = s;
}

void shaderInit()
{
	shader.load("shaders/phong.vs", "shaders/phong.fs");
	shader2.load("shaders/phong2.vs", "shaders/phong2.fs");

	changeShader(FLAT_SHADING);
}

void materialInit()
{
	model.initMaterial();
	//glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void lightInit()
{
	glEnable(GL_LIGHTING);
	
	for(int i = 0; i < MAX_LIGHTS; i++) {
		lights[i] = Light(GL_LIGHT0 + i);
	}
	lights[0].isOn = lights[5].isOn = lights[6].isOn = true;

	lights[0].updateRevolution(315, 1, 0, 0);
	lights[0].updateRevolution(45, 0, 1, 0);
	lights[1].updateRevolution(45, 1, 0, 0);
	lights[1].updateRevolution(45, 0, 1, 0);
	lights[2].updateRevolution(135, 1, 0, 0);
	lights[2].updateRevolution(45, 0, 1, 0);
	lights[3].updateRevolution(225, 1, 0, 0);
	lights[3].updateRevolution(45, 0, 1, 0);

	lights[4].updateRevolution(315, 1, 0, 0);
	lights[4].updateRevolution(315, 0, 1, 0);
	lights[5].updateRevolution(45, 1, 0, 0);
	lights[5].updateRevolution(315, 0, 1, 0);
	lights[6].updateRevolution(135, 1, 0, 0);
	lights[6].updateRevolution(315, 0, 1, 0);
	lights[7].updateRevolution(225, 1, 0, 0);
	lights[7].updateRevolution(315, 0, 1, 0);
}

void myInit()
{
	model.loadFromFile("models/balls.tri");
	controlMode = MODEL;
	printf("@@ Model\n");
	model.initTransform();
	camera.initTransform();
	camera.updateRevolution(45, 1, 0, 0);
    camera.updateRevolution(30, 0, 1, 0);
	shaderInit();
	materialInit();
	lightInit();

	autoRotate = true;
	printf("!! Enable model auto rotation\n");
	enableLights = true;
	printf("!! Enable lighting\n");
	printInfo = false;
	printf("!! Not printing info\n");

	glEnable(GL_DEPTH_TEST);
}

void idle()
{
	if(autoRotate) 
		model.updateRotation(1.0, 0.0, 1.0, 0.0);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(h < 100) h = 101;
	if(w < 100) w = 101;
	glViewport(50, 50, w - 100, h - 100);
	gluPerspective(60, 1.0 * (w - 100) / (h - 100), 1, 100000);

	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	// clear
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// setup camera transformation
	glLoadIdentity();
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, 
			  camera.lookAt.x,   camera.lookAt.y,   camera.lookAt.z, 
			  camera.up.x,       camera.up.y,       camera.up.z);
	
	// setup lights
	char tmps[100];
	if(enableLights) { 
		for(int i = 0; i < MAX_LIGHTS; i++) {
			if(lights[i].isOn) {
				lights[i].updatePosition();
				lights[i].updateColor();
				lights[i].updateAttenuation();
				glEnable(GL_LIGHT0 + i);
				if(shadingMode == PHONG_SHADING || shadingMode == PHONG_SHADING2) {
					sprintf(tmps, "lightIsOn[%d]", i);
					if(shadingMode == PHONG_SHADING)
						glUniform1i(glGetUniformLocation(shader.prog, tmps), 1);
					else
						glUniform1i(glGetUniformLocation(shader2.prog, tmps), 1);
				}
			}
			else {
				glDisable(GL_LIGHT0 + i);
				if(shadingMode == PHONG_SHADING || shadingMode == PHONG_SHADING2) {
					sprintf(tmps, "lightIsOn[%d]", i);
					if(shadingMode == PHONG_SHADING)
						glUniform1i(glGetUniformLocation(shader.prog, tmps), 0);
					else
						glUniform1i(glGetUniformLocation(shader2.prog, tmps), 0);
				}
			}
		}
	}
	else { 
		for(int i = 0; i < MAX_LIGHTS; i++) {
			if(lights[i].isOn) {
				glDisable(GL_LIGHT0 + i);
				if(shadingMode == PHONG_SHADING || shadingMode == PHONG_SHADING2) {
					sprintf(tmps, "lightIsOn[%d]", i);
					if(shadingMode == PHONG_SHADING)
						glUniform1i(glGetUniformLocation(shader.prog, tmps), 0);
					else
						glUniform1i(glGetUniformLocation(shader2.prog, tmps), 0);
				}
			}
		}
	}
	// setup model 
	glMultMatrixf(model.rotate.m);
	model.drawModel(1);

	glutSwapBuffers();
}

void keyboard(unsigned char key,int x,int y)
{
	switch(key) {
		case 27:
			exit(0);
			break;
		// mode
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			printf("@@ Light #%d\n", key - '0');
			controlMode = (ControlMode)(GL_LIGHT0 + (key - '0') - 1);
			lightControlMode = REVOLUTION;
			break;
		case '9':
			printf("@@ Model\n");
			controlMode = MODEL;
			break;
		case '0':
			printf("@@ Camera\n");
			controlMode = CAMERA;
			break;
		// switches
		case 'q':
			if(controlMode >= LIGHT0 && controlMode <= LIGHT7) {
				lights[controlMode - LIGHT0].isOn ^= 1;
				printf("!! Turn %s light #%d\n", ((lights[controlMode - LIGHT0].isOn) ? "on" : "off"), controlMode - LIGHT0);
			}
			break;
		case 'w':
			if(controlMode >= LIGHT0 && controlMode <= LIGHT7) {
				lightControlMode = (LightControlMode)(REVOLUTION + ((lightControlMode - REVOLUTION + 1) % 5));
				if(lightControlMode == REVOLUTION)
					printf("\t>> Light revolution control\n");
				else if(lightControlMode == ATTENUATION)
					printf("\t>> Light attenuation control\n");
				else if(lightControlMode == AMBIENT)
					printf("\t>> Light ambient color control\n");
				else if(lightControlMode == DIFFUSE)
					printf("\t>> Light diffuse color control\n");
				else 
					printf("\t>> Light specular color control\n");
			}
			break;
		case 's':
			changeShader((ShadingMode)((shadingMode + 1) % 4));
			break;
		case 'a':
			autoRotate ^= 1;
			printf("!! %s\n", (autoRotate) ? "Enable model auto rotation" :  "Disable model auto rotation");
			break;
		case 'd':
			enableLights ^= 1;
			printf("!! %s\n", (enableLights) ? "Enable lighting" :  "Disable lighting");
			break;
		case '?':
			printInfo ^= 1;
			printf("!! %s\n", (printInfo) ? "Printing info" :  "Not printing info");
			break;
		// change model
		case '[':
			model.loadFromFile("models/balls.tri");
			break;
		case ']':
			model.loadFromFile("models/Easter.tri");
			break;
		case ';':
			model.loadFromFile("models/Fighter.tri");
			break;
		case '\'':
			model.loadFromFile("models/Car_road.tri");
			break;
		case '.':
			model.loadFromFile("models/Csie.tri");
			break;
		case '/':
			model.loadFromFile("models/newcsieb1.tri");
			break;
		// transformation
		case 't':
			if(controlMode == MODEL)
				model.updateRotation(2.0, 1.0, 0.0, 0.0);
			else if(controlMode == CAMERA)
				camera.updateRevolution(camera.revAngle[0] + 3.0, 1, 0, 0);
			else {
				int idx = controlMode - LIGHT0;
				if(lightControlMode == REVOLUTION)
					lights[idx].updateRevolution(lights[idx].revAngle[0] + 1.0, 1, 0, 0);
				else if(lightControlMode == ATTENUATION)
					lights[idx].constAtt += 0.1;
				else if(lightControlMode == AMBIENT) 
					lights[idx].ambient.r = min(lights[idx].ambient.r + 0.01, 1.0); 
				else if(lightControlMode == DIFFUSE)
					lights[idx].diffuse.r = min(lights[idx].diffuse.r + 0.01, 1.0); 
				else 
					lights[idx].specular.r = min(lights[idx].specular.r + 0.01, 1.0); 
			}
			break;
		case 'y':
			if(controlMode == MODEL)
				model.updateRotation(-2.0, 1.0, 0.0, 0.0);
			else if(controlMode == CAMERA)
				camera.updateRevolution(camera.revAngle[0] - 3.0, 1, 0, 0);
			else {
				int idx = controlMode - LIGHT0;
				if(lightControlMode == REVOLUTION)
					lights[idx].updateRevolution(lights[idx].revAngle[0] - 1.0, 1, 0, 0);
				else if(lightControlMode == ATTENUATION)
					lights[idx].constAtt = max(lights[idx].constAtt - 0.1, 0.0);
				else if(lightControlMode == AMBIENT) 
					lights[idx].ambient.r = max(lights[idx].ambient.r - 0.01, 0.0); 
				else if(lightControlMode == DIFFUSE)
					lights[idx].diffuse.r = max(lights[idx].diffuse.r - 0.01, 0.0); 
				else 
					lights[idx].specular.r = max(lights[idx].specular.r - 0.01, 0.0); 
			}
			break;
		case 'g':
			if(controlMode == MODEL)
				model.updateRotation(2.0, 0.0, 1.0, 0.0);
			else if(controlMode == CAMERA)
				camera.updateRevolution(camera.revAngle[1] + 3.0, 0, 1, 0);
			else {
				int idx = controlMode - LIGHT0;
				if(lightControlMode == REVOLUTION)
					lights[idx].updateRevolution(lights[idx].revAngle[1] + 1.0, 0, 1, 0);
				else if(lightControlMode == ATTENUATION)
					lights[idx].linearAtt += 0.0001;
				else if(lightControlMode == AMBIENT) 
					lights[idx].ambient.g = min(lights[idx].ambient.g + 0.01, 1.0); 
				else if(lightControlMode == DIFFUSE)
					lights[idx].diffuse.g = min(lights[idx].diffuse.g + 0.01, 1.0); 
				else 
					lights[idx].specular.g = min(lights[idx].specular.g + 0.01, 1.0); 
			}
			break;
		case 'h':
			if(controlMode == MODEL)
				model.updateRotation(-2.0, 0.0, 1.0, 0.0);
			else if(controlMode == CAMERA)
				camera.updateRevolution(camera.revAngle[1] - 3.0, 0, 1, 0);
			else {
				int idx = controlMode - LIGHT0;
				if(lightControlMode == REVOLUTION)
					lights[idx].updateRevolution(lights[idx].revAngle[1] - 1.0, 0, 1, 0);
				else if(lightControlMode == ATTENUATION)
					lights[idx].linearAtt = max(lights[idx].linearAtt - 0.0001, 0.0);
				else if(lightControlMode == AMBIENT) 
					lights[idx].ambient.g = max(lights[idx].ambient.g - 0.01, 0.0); 
				else if(lightControlMode == DIFFUSE)
					lights[idx].diffuse.g = max(lights[idx].diffuse.g - 0.01, 0.0); 
				else 
					lights[idx].specular.g = max(lights[idx].specular.g - 0.01, 0.0); 
			}
			break;
		case 'b':
			if(controlMode == MODEL)
				model.updateRotation(2.0, 0.0, 0.0, 1.0);
			else if(controlMode == CAMERA)
				camera.updateRevolution(camera.revAngle[2] + 3.0, 0, 0, 1);
			else {
				int idx = controlMode - LIGHT0;
				if(lightControlMode == REVOLUTION)
					lights[idx].updateRevolution(lights[idx].revAngle[2] + 1.0, 0, 0, 1);
				else if(lightControlMode == ATTENUATION)
					lights[idx].quadAtt += 0.0000001;
				else if(lightControlMode == AMBIENT) 
					lights[idx].ambient.b = min(lights[idx].ambient.b + 0.01, 1.0); 
				else if(lightControlMode == DIFFUSE)
					lights[idx].diffuse.b = min(lights[idx].diffuse.b + 0.01, 1.0); 
				else 
					lights[idx].specular.b = min(lights[idx].specular.b + 0.01, 1.0); 
			}
			break;
		case 'n':
			if(controlMode == MODEL)
				model.updateRotation(-2.0, 0.0, 0.0, 1.0);
			else if(controlMode == CAMERA)
				camera.updateRevolution(camera.revAngle[2] - 3.0, 0, 0, 1);
			else {
				int idx = controlMode - LIGHT0;
				if(lightControlMode == REVOLUTION)
					lights[idx].updateRevolution(lights[idx].revAngle[2] - 1.0, 0, 0, 1);
				else if(lightControlMode == ATTENUATION)
					lights[idx].quadAtt = max(lights[idx].quadAtt - 0.0000001, 0.0);
				else if(lightControlMode == AMBIENT) 
					lights[idx].ambient.b = max(lights[idx].ambient.b - 0.01, 0.0); 
				else if(lightControlMode == DIFFUSE)
					lights[idx].diffuse.b = max(lights[idx].diffuse.b - 0.01, 0.0); 
				else 
					lights[idx].specular.b = max(lights[idx].specular.b - 0.01, 0.0); 
			}
			break;
		case 'm':
			if(controlMode == MODEL) {
				model.shininess = max(model.shininess - 0.5, 0.0);
				model.updateMaterial();
			}
			else if(controlMode == CAMERA) {
				camera.lookAtDist = max(camera.lookAtDist - 10, 0);
				camera.updateRevolution(0, 0, 0, 0);
			}
			else {
				lights[controlMode - LIGHT0].dist = max(lights[controlMode - LIGHT0].dist - 10, 0);
				lights[controlMode - LIGHT0].updateRevolution(0, 0, 0, 0);
			}
			break;
		case 'M':
			if(controlMode == MODEL) {
				model.shininess += 0.5;
				model.updateMaterial();
			}
			else if(controlMode == CAMERA) {
				camera.lookAtDist += 10;
				camera.updateRevolution(0, 0, 0, 0);
			}
			else {
				lights[controlMode - LIGHT0].dist += 10;
				lights[controlMode - LIGHT0].updateRevolution(0, 0, 0, 0);
			}
			break;
	}
	
	if(printInfo) {
		if(controlMode == MODEL)
			cout << model;
		else if(controlMode == CAMERA)
			cout << camera;
		else
			cout << lights[controlMode - LIGHT0];
	}

	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(960, 720);
	glutCreateWindow("ICG 2010 HW2 --- R99922020 Yu-Hsiang Huang");
	
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	
	glewInit();
	if(glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("GLSL not supported\n");
		exit(1);
	}
	printf("\n");

	myInit();
	glutMainLoop();

	return 0;
}
