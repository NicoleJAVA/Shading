// Include GLFW
#include <GL/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

mat4 ViewMatrix;
mat4 ProjectionMatrix;
mat4 ModelMatrix;

mat4 getViewMatrix(){
	return ViewMatrix;
}
mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

mat4 getModelMatrix(){
	return ModelMatrix;
}


// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
float deltaFoV = 0.0f;
float key_size = 1.0;
float k = 5.0;

vec3 key_trans = vec3(0.0, 0.0, 0.0);
vec3 key_rot = vec3(0.0, 0.0, 0.0);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	deltaFoV -= yoffset;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	// Scale up
	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
		key_size *= 1.1;
	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
		key_size *= 0.9;

	// Move camera forward
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		k *= 0.99;
	
	// Move camera backward
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		k *= 1.01;

	// Move object right
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		key_trans.x += 0.1;

	// Move object left
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		key_trans.x -= 0.1;

	// Move object up
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		key_trans.y += 0.1;

	// Move object down
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		key_trans.y -= 0.1;

	// Move object forward
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		key_trans.z += 0.1;

	// Move object backward
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		key_trans.z -= 0.1;

	// Rotate object in X axis
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		key_rot.x += 30;
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
		key_rot.x -= 30;

	//Rotate object in Y axis
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
		key_rot.y += 30;
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
		key_rot.y -= 30;

	//Rotate object in Z axis
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
		key_rot.z += 30;
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		key_rot.z -= 30;

	printf("size:%f\n",key_size);
	printf("translation:%f %f %f\n", key_trans.x, key_trans.y, key_trans.z);
	printf("rotation:%f %f %f\n", key_rot.x, key_rot.y, key_rot.z);
}

void computeModelMatrix(float size, vec3 center, vec3 trans, vec3 rot)
{
	mat4 ResizeMatrix = glm::scale(mat4(1.0), vec3(size, size, size));
	mat4 translateMatrix = translate(mat4(1.0), trans);
	mat4 rotateMatrix = rotate(mat4(1.0), rot.x, vec3(1.0, 0.0, 0.0));
	rotateMatrix = rotate(rotateMatrix, rot.y, vec3(0.0, 1.0, 0.0));
	rotateMatrix = rotate(rotateMatrix, rot.z, vec3(0.0, 0.0, 1.0));
	ModelMatrix = translateMatrix * rotateMatrix * ResizeMatrix;
	ModelMatrix = translate(ModelMatrix, -center);
}

void computeModelMatrixFromKey(vec3 center)
{
	mat4 ResizeMatrix = glm::scale(mat4(1.0), vec3(key_size, key_size, key_size));
	mat4 translateMatrix = translate(mat4(1.0), key_trans);
	mat4 rotateMatrix = rotate(mat4(1.0), key_rot.x, vec3(1.0, 0.0, 0.0));
	rotateMatrix = rotate(rotateMatrix, key_rot.y, vec3(0.0, 1.0, 0.0));
	rotateMatrix = rotate(rotateMatrix, key_rot.z, vec3(0.0, 0.0, 1.0));
	ModelMatrix = translateMatrix * rotateMatrix * ResizeMatrix;
	ModelMatrix = translate(ModelMatrix, -center);
}

void computeMatricesFromInputs(GLFWwindow* window){

	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	float FoV = initialFoV + deltaFoV;
	ProjectionMatrix = perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
}