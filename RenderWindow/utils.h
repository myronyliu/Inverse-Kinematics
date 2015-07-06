#pragma once

#include "stdafx.h"

// Utility functions
char * textFileRead(const char *);

// From http://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
#ifndef GLERROR_H
#define GLERROR_H
 
void _check_gl_error(const char *file, int line);
 
///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
 
#endif // GLERROR_H

std::string zero_padded_name(std::string, int, int);

glm::mat3 rotationMatrix(const glm::vec3& w);
glm::vec3 axisAngleVector(const glm::mat3& R);
glm::vec3 axisAngleVector(const glm::vec3& zIn, const glm::vec3& yIn);