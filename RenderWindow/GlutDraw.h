#ifndef GLUTDRAW_H
#define GLUTDRAW_H

#include "stdafx.h"

namespace GlutDraw
{
void drawLine(glm::vec3 tail, glm::vec3 head);

void drawCone(glm::vec3 base, float radius, glm::vec3 axis, int n = 32);

//TODO PRIO: drawRectangle so that we can do buttons.
void drawRectangle(float x, float y,
        float width, float height);

//Draw sphere at center of panel
void drawSphere(glm::vec3 center, float r, int n = 32, int m = 32);
};
#endif /* GLUTDRAW_H */