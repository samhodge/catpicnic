#ifndef SH_MATRIX_H
#define SH_MATRIX_H
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int setUniformMatrix(unsigned int programHandle, const char *uniformLabel, float *matrix);
int setUniformMatrix3(unsigned int programHandle, const char *uniformLabel, float *matrix);
#endif