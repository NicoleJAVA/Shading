//==================From here is your implementation ==========================//

	// Create and compile our GLSL program from the shaders. Todo: load shader
	GLuint programID2 = LoadShaders("phong.vertexshader", "simple_shading.fragmentshader");

	// Get a handle for uniform variables such as matrices, lights, ....
	
	GLuint ModelMatrixID2 = glGetUniformLocation(programID2, "ModelMatrix");
	GLuint ViewMatrixID2= glGetUniformLocation(programID2, "ViewMatrix");
	GLuint NormalMatrixID2 = glGetUniformLocation(programID2, "NormalMatrix");
	GLuint lightPosition_worldspaceID2 = glGetUniformLocation(programID2, "lightPosition_worldspace");
	GLuint lightColorID2 = glGetUniformLocation(programID2, "lightColor");

	// Get a handle for vertex attribute arrays such as vertex positions, colors, normals, ....
	GLuint vertexPosition_modelspaceID2 = glGetAttribLocation(programID2, "vertexPosition_modelspace");
	GLuint vertexColorID2 = glGetAttribLocation(programID2, "vertexColor");
	GLuint vertexNormal_modelspaceID2 = glGetAttribLocation(programID2, "vertexNormal_modelspace");

	// Load your scene
	TRIModel model2;
	model2.loadFromFile("models/Balls.tri");
	
    
	/*** Here shows an exmple which generates three different vertex buffers for vertices, colors, normals.***/
	//However, you can always incorporate them together in a single buffer.(Hint: use glBufferSubData)

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, model2.vertices.size() * sizeof(vec3), &model2.vertices[0], GL_STATIC_DRAW);

	GLuint colorbuffer2;
	glGenBuffers(1, &colorbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
	glBufferData(GL_ARRAY_BUFFER, model2.vertices.size() * sizeof(vec3), &model2.foreColors[0], GL_STATIC_DRAW);

	GLuint normalbuffer2;
	glGenBuffers(1, &normalbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
	glBufferData(GL_ARRAY_BUFFER, model2.vertices.size() * sizeof(vec3), &model2.normals[0], GL_STATIC_DRAW);

	/*** **/

	GLfloat rotZ2 = 0;
    while (!glfwWindowShouldClose(window))
    {
	/**************************************/
	//for( int i = 1; i <= 2; i++ ){   //   <<	Nicole edit
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Tell OpenGL to use the shader program 
		glUseProgram(programID2);
		
		//send matrix to shader: You can use the keyboard and mouse to close the window and control the transformation
		//		                 (See computeMatricesFromKey(window) )
		mat4 ViewMatrix2 = lookAt( vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1.0, 0.0) );
		//move object to the world origin
		mat4 ModelMatrix2 = translate( mat4(1.0), vec3( 0.5, 0.5, 0.5 ) );
		ModelMatrix2 = rotate( ModelMatrix2, rotZ2, vec3(0.0, 0.0, 1.0) );
		ModelMatrix2 = scale( ModelMatrix2, vec3(0.001, 0.001, 0.001) );
		ModelMatrix2 = translate(ModelMatrix2, -vec3(model2.center[0], model2.center[1], model2.center[2]));
		// compute vertex normal in camera space (transpose of inverse View * Model)
		// You can compute it in GLSL if your GPU support OpenGL 3.x
		mat4 NormalMatrix2 = transpose( inverse(ViewMatrix2 * ModelMatrix2) );
		glUniformMatrix4fv(ViewMatrixID2, 1, GL_FALSE, &ViewMatrix2[0][0]);
		glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &ModelMatrix2[0][0]);
		glUniformMatrix4fv(NormalMatrixID2, 1, GL_FALSE, &NormalMatrix2[0][0]);

		/*** adjust transformation by key ***/
		/*computeMatricesFromInputs(window);
		computeModelMatrixFromKey(vec3(model2.center[0], model2.center[1], model2.center[2]));
		mat4 ModelMatrix = getModelMatrix();*/
		/*** ****/

		//send lights to shader
		vec3 lightPosition = vec3(4, 4, 4);
		vec3 lightColor = vec3(1.0, 1.0, 1.0);
		glUniform3f(lightPosition_worldspaceID2, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(lightColorID2, lightColor.r, lightColor.g, lightColor.b);
	
		//define an vertex attribute array on the vertex buffer
		glEnableVertexAttribArray(vertexPosition_modelspaceID2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(
			vertexPosition_modelspaceID2, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		//define an vertex attribute array on the color buffer
		glEnableVertexAttribArray(vertexColorID2);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(
			vertexColorID2, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		//define an vertex attribute array on the normal buffer
		glEnableVertexAttribArray(vertexNormal_modelspaceID2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
		glVertexAttribPointer(
			vertexNormal_modelspaceID2, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		//draw the scene by vertex attribute arrays
		glDrawArrays(GL_TRIANGLES, 0, model2.vertices.size());
		glDisableVertexAttribArray(vertexPosition_modelspaceID2);
		glDisableVertexAttribArray(vertexColorID2);
		glDisableVertexAttribArray(vertexNormal_modelspaceID2);
        glfwSwapBuffers(window);
        glfwPollEvents();
		
		//update rotate angle
		rotZ2 += 0.1;


		/**************************************/
	}  // End for << Nicole edit

    }
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteBuffers(1, &colorbuffer2);
	glDeleteBuffers(1, &normalbuffer2);
	glDeleteProgram(programID2);

	//===================Stop your implementation here ================================//


