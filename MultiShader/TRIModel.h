#include <iostream>
#include <vector>

#pragma once

using namespace std;
using namespace glm;


class TRIModel{
	
public:
	double center[3];
	bool loadFromFile(const char* fileName);
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec3> foreColors;
	vector<vec3> backColors;
	TRIModel();
	~TRIModel();

};