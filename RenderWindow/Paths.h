#ifndef _PATHS_H
#define _PATHS_H

#include "stdafx.h"

namespace PathParameterizations {

    glm::vec3 line(const float& t) { return (1 - t)*glm::vec3(0, 0, 0) + t*glm::vec3(0, 0, 1); }
    glm::vec3 circle(const float& t) { return glm::vec3(cos(2 * M_PI*t), sin(2 * M_PI*t), 0); }
}

#endif