#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs(GLFWwindow* window);
void computeModelMatrix(float size, glm::vec3 center, glm::vec3 trans, glm::vec3 rot);
void computeModelMatrixFromKey(glm::vec3 center);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::mat4 getModelMatrix();
glm::vec3 getCameraPosition();

#endif