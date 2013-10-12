#include <glm/glm.hpp>
#include <TRIModel.h>


bool TRIModel::loadFromFile(const char* fileName){
	char tmp_string[100] = "";
	double max[3] = {-DBL_MAX}, min[3] = {DBL_MAX};
	FILE* inFile = fopen(fileName, "r");
	if(!inFile){
		cout << "Can not open object File \"" << fileName << "\" !" << endl;
		return false;
	}

	cout <<"Loading \"" << fileName << "\" !" << endl;
	while(fscanf(inFile,"%s",tmp_string) != EOF){
		vec3 tmp_vertex;
		vec3 tmp_normal;
		int tmp_int[6];
		vec3 tmp_fColor;
		vec3 tmp_bColor;
		fscanf(inFile,"%d %d %d %d %d %d",&tmp_int[0], &tmp_int[1], &tmp_int[2], &tmp_int[3], &tmp_int[4], &tmp_int[5]);
		for(int i = 0; i < 3; ++i){
			tmp_fColor[i] = tmp_int[i] / 255.0;
			tmp_bColor[i] = tmp_int[i+3] / 255.0;
		}
		for(int i = 0; i < 3; i++){
			fscanf(inFile,"%f %f %f %f %f %f",&tmp_vertex.x,&tmp_vertex.y, &tmp_vertex.z, &tmp_normal.x, &tmp_normal.y, &tmp_normal.z);
			vertices.push_back(tmp_vertex);
			normals.push_back(tmp_normal);
			foreColors.push_back(tmp_fColor);
			backColors.push_back(tmp_bColor);
			for(int j = 0; j < 3; j++){
				if(tmp_vertex[j] < min[j]){
					min[j] = tmp_vertex[j];
				}
				if(tmp_vertex[j] > max[j]){
					max[j] = tmp_vertex[j];
				}
			}
		}
	}
	for(int i = 0; i < 3; i++)
		center[i] = (min[i] + max[i]) / 2;
	return true;
}


TRIModel::TRIModel(){
}

TRIModel::~TRIModel(){
}