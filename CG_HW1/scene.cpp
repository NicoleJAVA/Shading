/*
 * NTUCSIE ICG2010
 * R99922020 Yu-Hsiang Huang
 */

#include "scene.h"
#include <iostream>
#include <fstream>
using namespace std;
#include <GL/glut.h>

#define MIN_DIAG_LEN 800

Camera::Camera()
{
	initTransform();
}

void Camera::initTransform() 
{
	lookAtDist = 1000;
	// position
	position[0] = position[1] = 0.0;  position[2] = lookAtDist;
	// look at
	lookAt[0] = lookAt[1] = lookAt[2] = 0.0;
	// up vector
	up[0] = up[2] = 0.0;  up[1] = 1.0;
	// angles
	revAngle[0] = revAngle[1] = revAngle[2] = 0.0;
	//rotate
	rotate.setIdentity();
}

void Camera::updateRotation(float angle, float x, float y, float z)
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glRotatef(angle, x, y, z);
	glMultMatrixf(rotate.m);
    glGetFloatv(GL_MODELVIEW_MATRIX, rotate.m);
	glPopMatrix();
}

void Camera::updateRevolution(float angle, int x, int y, int z) 
{
	if(x == 1) 
		revAngle[0] = (int)angle % 360;
	else if(y == 1)
		revAngle[1] = (int)angle % 360;

	float yaw = toRad(revAngle[0]);
	float pitch = toRad(revAngle[1]);
	
	position.x = lookAtDist * cosf(pitch) * sinf(yaw) - lookAt[0];
	position.y = lookAtDist * sinf(pitch)             - lookAt[1];
	position.z = lookAtDist * cosf(pitch) * cosf(yaw) - lookAt[2];

	up.x = -sinf(pitch) * sinf(yaw);
	up.y =  cosf(pitch);
	up.z = -sinf(pitch) * cosf(yaw);
}



Model::Model()
{
	initTransform();
	initMaterial();
}

bool Model::loadFromFile(const char* fileName)
{
	char str[20] = {0};
	max.x = max.y = max.z = -100000.0;
	min.x = min.y = min.z = 100000.0;

	fstream inFile(fileName, fstream::in);
	if(!inFile){
		cout << "Can not open object File \"" << fileName << "\" !" << endl;
		return false;
	}

	// clean up the old data
	triangleList.clear();

	cout << "Loading \"" << fileName << "\" ... ";
	while(1) {
		// model mesh type
		inFile >> str;
		if(inFile.eof())
			break;
		// read a triangle
		Triangle triTmp;
		inFile >> triTmp;
		if(triTmp.vertex[0].x == 0.0 && triTmp.vertex[0].y == 0.0 && triTmp.vertex[0].z == 0.0)
			printf("%d\n", triangleList.size());
		// for each of the 3 vetices
		for(int j = 0; j < 3; j++) {
			// x, y, z
			for(int k = 0; k < 3; k++) {
				if(triTmp.vertex[j][k] < min[k])
					min[k] = triTmp.vertex[j][k];
				if(triTmp.vertex[j][k] > max[k])
					max[k] = triTmp.vertex[j][k];
			}
		}
		triangleList.push_back(triTmp);
	}
	inFile.close();

	// calculate center
	center = (min + max) / 2;
	// center the model
	max -= center;
	min -= center;
	for(int i = 0; i < triangleList.size(); i++) 
		for(int j = 0; j < 3; j++) 
			triangleList[i].vertex[j] -= center;
	center.x = center.y = center.z = 0.0;

	// upscale uniformly
	float diag = (max - min).length();
	if(diag < MIN_DIAG_LEN)
		unifromScale(MIN_DIAG_LEN / diag);
	
	cout << "done!!" << endl;
	return true;
}

void Model::copy(Model * t)
{
	for(int i = 0; i < 3; i++){
		center[i] = t->center[i];
		max[i] = t->max[i];
		min[i] = t->min[i];
	}
	triangleList = t->triangleList;
}

void Model::unifromScale(float s)
{
	if(s == 0.0)	return;

	for(int i = 0; i < 3; i++){
		min[i] *= s;
		max[i] *= s;
	}
	for(int i = 0; i < triangleList.size(); i++) {
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 3; k++) {
				triangleList[i].vertex[j][k] *= s;
			}
		}
	}
}

void Model::drawAxes()
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glBegin(GL_LINES);
		// x axis
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(300.0, 0.0, 0.0);
		// y axis
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 300.0, 0.0);
		// z axis
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 300.0);
	glEnd();
	glPopMatrix();
}

void Model::drawBBox()
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex3f(max.x, max.y, max.z);
		glVertex3f(max.x, min.y, max.z);
		glVertex3f(max.x, min.y, min.z);
		glVertex3f(max.x, max.y, min.z);

		glVertex3f(max.x, max.y, max.z);
		glVertex3f(min.x, max.y, max.z);
		glVertex3f(min.x, min.y, max.z);
		glVertex3f(max.x, min.y, max.z);

		glVertex3f(max.x, max.y, max.z);
		glVertex3f(min.x, max.y, max.z);
		glVertex3f(min.x, max.y, min.z);
		glVertex3f(max.x, max.y, min.z);
		
		glVertex3f(min.x, min.y, min.z);
		glVertex3f(min.x, max.y, min.z);
		glVertex3f(min.x, max.y, max.z);
		glVertex3f(min.x, min.y, max.z);

		glVertex3f(min.x, min.y, min.z);
		glVertex3f(max.x, min.y, min.z);
		glVertex3f(max.x, max.y, min.z);
		glVertex3f(min.x, max.y, min.z);

		glVertex3f(min.x, min.y, min.z);
		glVertex3f(max.x, min.y, min.z);
		glVertex3f(max.x, min.y, max.z);
		glVertex3f(min.x, min.y, max.z);
	glEnd();
	glPopMatrix();
}

void Model::drawModel(int mode)
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	if(mode == 0)
		glBegin(GL_LINES);
	else
		glBegin(GL_TRIANGLES);
	for(int i = 0; i < triangleList.size(); i++) {
		// set color
		glColor3fv(triangleList[i].frontColor.vec());
		for(int j = 0; j < 3; j++) {
			// set normal
			glNormal3fv(triangleList[i].normal[j].vec());
			if(mode == 0) {
				glVertex3fv(triangleList[i].vertex[j].vec());
				glVertex3fv(triangleList[i].vertex[(j + 1) % 3].vec());
			}
			else
				glVertex3fv(triangleList[i].vertex[j].vec());
		}
	}
	glEnd();
	glPopMatrix();
}

void Model::initTransform()
{
	// translation
	translate[0] = translate[1] = translate[2] = 0.0;
	// scaling
	scale[0] = scale[1] = scale[2] = 1.0;
	// shear
	shear.setIdentity();
	// rotation
	rotate.setIdentity();
}

void Model::initMaterial() 
{
	shininess = 20.0;
	ambient =  Vector(0.15, 0.15, 0.15, 1.0);
	diffuse =  Vector(0.8,  0.8,  0.8,  1.0);
	specular = Vector(1.0,  1.0,  1.0,  1.0);
	updateMaterial();
}

void Model::updateMaterial() 
{
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.vec());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.vec());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.vec());
}

void Model::updateShear(float angle, int to, int from) 
{
	float tmp[16] = {0};
	tmp[0] = tmp[5] = tmp[10] = tmp[15] = 1.0;
	tmp[from * 4 + to] = tan(toRad(angle));

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(tmp);
	glMultMatrixf(shear.m);
    glGetFloatv(GL_MODELVIEW_MATRIX, shear.m);
	glPopMatrix();
}

void Model::updateRotation(float angle, float x, float y, float z, bool useWorldCoord) 
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	if(useWorldCoord) {
		glRotatef(angle, x, y, z);
		glMultMatrixf(rotate.m);
	}
	else {
		glMultMatrixf(rotate.m);
		glRotatef(angle, x, y, z);
	}
    glGetFloatv(GL_MODELVIEW_MATRIX, rotate.m);
	glPopMatrix();
}



Light::Light(int idx)
{
	index = idx;
	isOn = false;
	glEnable(index);

	initTransform();
	initLightParam();
}

void Light::initTransform()
{
	dist = 1000;
	position[0] = position[1] = 0.0;  position[2] = dist;  position[3] = 1.0;
	revAngle[0] = revAngle[1] = revAngle[2] = 0.0;
}

void Light::initLightParam() 
{
	ambient =  Vector(0.1, 0.1, 0.1, 1.0);
	diffuse =  Vector(0.8, 0.8, 0.8, 1.0);
	specular = Vector(1.0, 1.0, 1.0, 1.0);
	updateColor();

	constAtt = 1.0;
	linearAtt = 0.0001;
	quadAtt = 0.0000001;
	updateAttenuation();
}

void Light::updatePosition() 
{
	glLightfv(index, GL_POSITION, position.vec());
}

void Light::updateColor() 
{
	glLightfv(index, GL_AMBIENT, ambient.vec());
	glLightfv(index, GL_DIFFUSE, diffuse.vec());
	glLightfv(index, GL_SPECULAR, specular.vec());
}

void Light::updateAttenuation() 
{
	glLightf(index, GL_CONSTANT_ATTENUATION, constAtt);
	glLightf(index, GL_LINEAR_ATTENUATION, linearAtt);
	glLightf(index, GL_QUADRATIC_ATTENUATION, quadAtt);
}

void Light::updateRevolution(float angle, int x, int y, int z) 
{
	if(x == 1)
		revAngle[0] = angle;
	else if(y == 1)
		revAngle[1] = angle;

	float yaw = toRad(revAngle[0]);
	float pitch = toRad(revAngle[1]);
	
	position.x = dist * cosf(pitch) * sinf(yaw);
	position.y = dist * sinf(pitch);
	position.z = dist * cosf(pitch) * cosf(yaw);
}
   


ostream &operator<< (ostream &os, const Camera &c)
{
	os << "====================================================" << endl;
	os << "Camera information:" << endl;
	os << "\tlookAtDist = " << c.lookAtDist << endl;
	os << "\tposition = (" << c.position.x << ", " << c.position.y << ", " << c.position.z << ")" << endl;
	os << "\tlookAt = (" << c.lookAt.x << ", " << c.lookAt.y << ", " << c.lookAt.z << ")" << endl;
	os << "\tup = (" << c.up.x << ", " << c.up.y << ", " << c.up.z << ")" << endl;
	os << "\trevAngle = (" << c.revAngle.x << ", " << c.revAngle.y << ", " << c.revAngle.z << ")" << endl;
	os << "====================================================" << endl;
    return os;
}

ostream &operator<< (ostream &os, const Model &m)
{
	os << "====================================================" << endl;
	os << "Model (material) information:" << endl;
	os << "\tshininess = " << m.shininess << endl;
	os << "\tambient = (" << m.ambient.r << ", " << m.ambient.g << ", " << m.ambient.b << ")" << endl;
	os << "\tdiffuse = (" << m.diffuse.r << ", " << m.diffuse.g << ", " << m.diffuse.b << ")" << endl;
	os << "\tspecular = (" << m.specular.r << ", " << m.specular.g << ", " << m.specular.b << ")" << endl;
	os << "====================================================" << endl;
    return os;
}

ostream &operator<< (ostream &os, const Light &l)
{
	os << "====================================================" << endl;
	os << "Light #"<< (l.index - GL_LIGHT0) + 1 << " information:" << endl;
	os << "\tisOn = " << l.isOn << endl;
	os << "\tdist = " << l.dist << endl;
	os << "\tposition = (" << l.position.x << ", " << l.position.y << ", " << l.position.z << ")" << endl;
	os << "\trevAngle = (" << l.revAngle.x << ", " << l.revAngle.y << ", " << l.revAngle.z << ")" << endl;
	os << "\tambient = (" << l.ambient.r << ", " << l.ambient.g << ", " << l.ambient.b << ")" << endl;
	os << "\tdiffuse = (" << l.diffuse.r << ", " << l.diffuse.g << ", " << l.diffuse.b << ")" << endl;
	os << "\tspecular = (" << l.specular.r << ", " << l.specular.g << ", " << l.specular.b << ")" << endl;
	os << "\tconstAtt = " << l.constAtt << endl;
	os << "\tlinearAtt = " << l.linearAtt << endl;
	os << "\tquadAtt = " << l.quadAtt << endl;
	os << "====================================================" << endl;
    return os;
}