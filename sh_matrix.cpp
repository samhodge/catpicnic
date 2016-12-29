#include "sh_matrix.h"
/**
 * Sends a single 4x4 matrix of floats to the shader program
 * @param programHandle, shader program handle
 * @param uniformLable, null terminated string with the name of uniform in the shader program
 * @param matrix, array of 16 floats to send to shader program
 * @returns 0 for success, error otherwise
 */
int setUniformMatrix(unsigned int programHandle, const char *uniformLabel, float *matrix) {

	// Get the uniform from the shader program
	// All active uniforms can be found by name using this function
	int uniformHandle = glGetUniformLocation(programHandle, uniformLabel);

	if (uniformHandle == -1) {
		std::cout << "Uniform: " << uniformLabel << " was not an active uniform label\n";
		return 1;
	}

	// Set its value to the supplied matrix
	// Note the function suffix *Matrix4fv, meaning a 4x4 matrix of floats
	// There exists many forms of this function for sending different data-types/amounts
	glUniformMatrix4fv( uniformHandle, 1, false, matrix );

	return 0;
}

int setUniformMatrix3(unsigned int programHandle, const char *uniformLabel, float *matrix) {

	// Get the uniform from the shader program
	// All active uniforms can be found by name using this function
	int uniformHandle = glGetUniformLocation(programHandle, uniformLabel);

	if (uniformHandle == -1) {
		std::cout << "Uniform: " << uniformLabel << " was not an active uniform label\n";
		return 1;
	}

	// Set its value to the supplied matrix
	// Note the function suffix *Matrix4fv, meaning a 4x4 matrix of floats
	// There exists many forms of this function for sending different data-types/amounts
	glUniformMatrix3fv( uniformHandle, 1, false, matrix );

	return 0;
}