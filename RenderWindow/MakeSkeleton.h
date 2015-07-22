#ifndef _MAKESKELETON_H_
#define _MAKESKELETON_H_

#include "Body.h"

Scene::Body* axisTree(const int& depth = 2);

std::pair<Scene::Body*,Scene::Bone*> chain(const int& nJoints = 5);

#endif