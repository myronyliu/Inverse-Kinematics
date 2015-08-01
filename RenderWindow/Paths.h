#ifndef _PATHS_H
#define _PATHS_H

#include "stdafx.h"

namespace PathParameterizations {

    glm::vec3 line(const float& t) {
        float s;
        if (t < 0.5) s = 2 * t;
        else s = 2 * (1 - t);
        return (1 - s)*glm::vec3(0, 0, -0.5) + s*glm::vec3(0, 0, 0.5);
    }
    glm::vec3 circle(const float& t) { return glm::vec3(cos(2 * M_PI*t), sin(2 * M_PI*t), 0); }
    glm::vec3 cardioid(const float& t) {
        float s = 0.5f + sin(2 * M_PI*t);
        return s*glm::vec3(cos(2 * M_PI*t), sin(2 * M_PI*t), 0);
    }
}

#endif