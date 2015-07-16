#ifndef GLUTDRAW_H
#define GLUTDRAW_H

#include "stdafx.h"

namespace GlutDraw
{
    void drawLine(glm::vec3 tail, glm::vec3 head);

    void drawCone(glm::vec3 base, float radius, glm::vec3 axis, int n = 32);

    void drawParallelogram(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis);

    void drawRectangle(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis);

    void drawSphere(glm::vec3 center, glm::vec3 axis, int n = 32, int m = 32);

    void drawDome(glm::vec3 center, glm::vec3 axis, float thetaMax = M_PI / 2, bool solid = true, bool outwardNormals = true, int m = 32, int n = 32);

    void drawDomeShell(glm::vec3 center, glm::vec3 axis, float thetaMax = M_PI / 2, float thicknessRatio = 1.1f, int m = 32, int n = 32);

    void drawCylinder(glm::vec3 center, glm::vec3 halfAxis, float r, int n = 32);

    void drawParallelepiped(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis, glm::vec3 zAxis);

    void drawPyramid(glm::vec3 base, glm::vec3 baseToTip, glm::vec3 baseToCorner, int nFaces = 4, bool mode = true);

    void drawDoublePyramid(glm::vec3 base, glm::vec3 baseToTip, glm::vec3 baseToCorner, int nFaces = 4, bool mode = true);
};

#endif /* GLUTDRAW_H */