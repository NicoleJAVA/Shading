#include "CG_HW1.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>

#include <GL/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp>
#include <controls.hpp>
#include <TRIModel.h>

using namespace glm;
vector<TRIModel> models;

int main( void )
{
    GLFWwindow* window;
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(768, 768, "simple shading", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
	if(glewInit() != GLEW_OK)
		cout << "Failed to initial GLEW"<<endl;
   
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	//==================From here is your implementation ==========================//

	// Create and compile our GLSL program from the shaders. Todo: load shader
	GLuint programID = LoadShaders("simple_shading.vertexshader", "simple_shading.fragmentshader");

	// Get a handle for uniform variables such as matrices, lights, ....
	GLuint ModelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
	GLuint NormalMatrixID = glGetUniformLocation(programID, "NormalMatrix");
	GLuint lightPosition_worldspaceID = glGetUniformLocation(programID, "lightPosition_worldspace");
	GLuint lightColorID = glGetUniformLocation(programID, "lightColor");

	// Get a handle for vertex attribute arrays such as vertex positions, colors, normals, ....
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	// Load your scene
	TRIModel model;
	model.loadFromFile("models/Balls.tri");
	
    
	/*** Here shows an exmple which generates three different vertex buffers for vertices, colors, normals.***/
	//However, you can always incorporate them together in a single buffer.(Hint: use glBufferSubData)

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(vec3), &model.vertices[0], GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(vec3), &model.foreColors[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(vec3), &model.normals[0], GL_STATIC_DRAW);

	/*** **/

	GLfloat rotZ;
    while (!glfwWindowShouldClose(window))
    {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Tell OpenGL to use the shader program 
		glUseProgram(programID);
		
		//send matrix to shader: You can use the keyboard and mouse to close the window and control the transformation
		//		                 (See computeMatricesFromKey(window) )
		mat4 ViewMatrix = lookAt( vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1.0, 0.0) );
		//move object to the world origin
		mat4 ModelMatrix = translate(mat4(1.0), -vec3(model.center[0], model.center[1], model.center[2]));
		ModelMatrix = scale( ModelMatrix, vec3(0.001, 0.001, 0.001) );
		ModelMatrix = rotate( ModelMatrix, rotZ, vec3(0.0, 0.0, 1.0) );
		// compute vertex normal in camera space (transpose of inverse View * Model)
		// You can compute it in GLSL if your GPU support OpenGL 3.x
		mat4 NormalMatrix = transpose( inverse(ViewMatrix * ModelMatrix) );
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(NormalMatrixID, 1, GL_FALSE, &NormalMatrix[0][0]);

		//send lights to shader
		vec3 lightPosition = vec3(4, 4, 4);
		vec3 lightColor = vec3(1.0, 1.0, 1.0);
		glUniform3f(lightPosition_worldspaceID, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(lightColorID, lightColor.r, lightColor.g, lightColor.b);
	
		//define an vertex attribute array on the vertex buffer
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		//define an vertex attribute array on the color buffer
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			vertexColorID, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		//define an vertex attribute array on the normal buffer
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		//draw the scene by vertex attribute arrays
		glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexColorID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);
        glfwSwapBuffers(window);
        glfwPollEvents();
		
		//update rotate angle
		rotZ += 0.1;
    }
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);

	//===================Stop your implementation here ================================//

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}

