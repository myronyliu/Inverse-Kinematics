#ifndef GLUTDRAW_H
#define GLUTDRAW_H

#include "stdafx.h"

namespace GlutDraw
{
    void drawLine(glm::vec3 tail, glm::vec3 head);

    void drawCone(glm::vec3 base, float radius, glm::vec3 axis, int n = 32);

    void drawParallelogram(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis);

    void drawRectangle(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis);

    void drawSphere(glm::vec3 center, glm::vec3 axis, int n = 16, int m = 16);

    void drawDome(glm::vec3 center, glm::vec3 axis, float thetaMax = M_PI / 2, bool solid = true, bool outwardNormals = true, int m = 16, int n = 16);

    void drawDomeShell(glm::vec3 center, glm::vec3 axis, float thetaMax = M_PI / 2, float thicknessRatio = 1.1f, int m = 16, int n = 16);

    void drawCylinder(glm::vec3 center, glm::vec3 halfAxis, float r, int n = 16);

    void drawParallelepiped(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis, glm::vec3 zAxis);

    void drawPyramid(glm::vec3 base, glm::vec3 baseToTip, glm::vec3 baseToCorner, int nFaces = 4, bool mode = true);

    void drawDoublePyramid(glm::vec3 base, glm::vec3 baseToTip, glm::vec3 baseToCorner, int nFaces = 4, bool mode = true);
    
    void drawExhaustiveTriangles(const std::vector<glm::vec3>& vertices);

    void drawTriangularPrism(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& axis);

    void drawWedge(
        glm::vec3 center, glm::vec3 zAxis, glm::vec3 yAxis, float phiRange, bool solid = true,
        bool outwardNormals = true, int nThetaDivisions = 16, int nPhiDivisionsFull = 16
        );
    void drawWedgeShell(
        glm::vec3 center, glm::vec3 zAxis, glm::vec3 yAxis, float phiRange, float radiusRatio,
        int nThetaDivisions = 16, int nPhiDivisionsFull = 16
        );
};

#endif /* GLUTDRAW_H */