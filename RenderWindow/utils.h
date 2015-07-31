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

template <class T>
T* pointerToCopy(const T&); // DON'T FORGET TO FREE MEMORY AFTER YOU ARE DONE WITH USING THE RESULT
template <class T>
T* pointerToCopy(const T*);

void SaveAsBMP(const char* fileName);

void printMat3(const glm::mat3&);
void printVec3(const glm::vec3&, const bool& = true);
void printVec2(const glm::vec2&, const bool& = true);

void printVector(const std::vector<float>&);

void pushTranslation3(const glm::vec3&);
void pushRotation3(const glm::vec3&);

void pushTranslation(const glm::vec3&);
void pushRotation(const glm::vec3&);