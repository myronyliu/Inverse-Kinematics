#include "GlutDraw.h"
#include "Math.h"
#include "utils.h"

using namespace Math;

void GlutDraw::drawLine(glm::vec3 tail, glm::vec3 head)
{
    glBegin(GL_LINES);
    glVertex3f(tail[0], tail[1], tail[2]);
    glVertex3f(head[0], head[1], head[2]);
    glEnd();
}

void GlutDraw::drawCone(glm::vec3 base, float r, glm::vec3 axis, int n) {

    float h = glm::length(axis);
    glm::vec3 baseNormal = -axis / h;
    glm::vec3 w = axisAngleAlignZtoVEC3(-baseNormal);
    float angle = glm::length(w);
    glm::vec3 wHat = glm::vec3(0, 0, 1);
    if (angle > 0) wHat = w / angle;

    glm::vec3 tip = base + axis;
    float dTheta = 2 * M_PI / n;

    auto normal = [&](const float& theta) {
        glm::vec3 n(cos(theta), sin(theta), r / h);
        n = glm::normalize(glm::rotate(n, angle, wHat));
        glNormal3f(n[0], n[1], n[2]);
    };
    auto vertex = [&](const float& theta) {
        glm::vec3 v = glm::vec3(r*cos(theta), r*sin(theta), 0);
        glVertex3f(base[0] + v[0], base[1] + v[1], base[2] + v[2]);
    };

    glBegin(GL_TRIANGLES);
    for (int i = 0; i <= n; i++) {
        float theta = i*dTheta;
        normal(theta + dTheta/2);
        glVertex3f(tip[0], tip[1], tip[2]);
        normal(theta);
        vertex(theta);
        normal(theta + dTheta);
        vertex(theta + dTheta);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(baseNormal[0], baseNormal[1], baseNormal[2]);
    glVertex3f(base[0], base[1], base[2]);
    for (int i = 0; i <= n; i++) {
        vertex(i*dTheta);
    }
    glEnd();
}

void GlutDraw::drawParallelogram(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis)
{
    glm::vec3 pt00 = center - xAxis - yAxis;
    glm::vec3 pt01 = center - xAxis + yAxis;
    glm::vec3 pt11 = center + xAxis + yAxis;
    glm::vec3 pt10 = center + xAxis - yAxis;

    glm::vec3 normal = glm::normalize(glm::cross(xAxis, yAxis));

    glBegin(GL_QUADS);
    glNormal3f(normal[0], normal[1], normal[2]);
    glVertex3f(pt00[0], pt00[1], pt00[2]);
    glVertex3f(pt01[0], pt01[1], pt01[2]);
    glVertex3f(pt11[0], pt11[1], pt11[2]);
    glVertex3f(pt10[0], pt10[1], pt10[2]);
    glEnd();
}

void GlutDraw::drawRectangle(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxisDiag) {
    glm::vec3 yAxis = yAxisDiag - glm::dot(yAxisDiag, xAxis)*xAxis;
    drawParallelogram(center, xAxis, yAxis);
}

void GlutDraw::drawSphere(glm::vec3 center, glm::vec3 axis, int m, int n) {
    //GlutDraw::drawDome(center, axis, M_PI, false, true, m, n);

    glPushMatrix();
    pushTranslation(center);
    pushRotation(axisAngleAlignZtoVEC3(axis));
    glutSolidSphere(glm::length(axis), m, n);
    glPopMatrix();
}

void GlutDraw::drawDome (
    glm::vec3 center, glm::vec3 axis, float thetaMaxIn, bool solid, bool outwardNormals, int nThetaDivisionsFull, int nPhiDivisions)
{
    float r = glm::length(axis);
    float thetaMax = Math::clamp(0.0f, thetaMaxIn, M_PI);
    if (r == 0 || thetaMax == 0 || nPhiDivisions < 2 || nThetaDivisionsFull < 3) return;

    int nThetaDivisions = ceil((thetaMax / M_PI)*nThetaDivisionsFull);

    float dTheta = thetaMax / nThetaDivisions;
    float dPhi = 2 * M_PI / nPhiDivisions;

    glm::vec3 w = axisAngleAlignZtoVEC3(axis);
    float angle = glm::length(w);
    glm::vec3 wHat(0, 0, 1);
    if (angle > 0) wHat = w / angle;
    
    auto sphereNormal = [&](const float& theta, const float& phi) {
        glm::vec3 dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
        glm::vec3 n = glm::rotate(dir, angle, wHat);
        if (outwardNormals) glNormal3f(n[0], n[1], n[2]);
        else glNormal3f(-n[0], -n[1], -n[2]);
    };
    auto sphereVertex = [&](const float& theta, const float& phi) {
        glm::vec3 dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
        glm::vec3 n = glm::rotate(dir, angle, wHat);
        glVertex3f(center[0] + r*n[0], center[1] + r*n[1], center[2] + r*n[2]);
    };

    glBegin(GL_TRIANGLE_FAN);
    sphereNormal(0,0);
    sphereVertex(0,0);
    for (int j = 0; j <= nPhiDivisions; j++) {
        float phi = j*dPhi;
        sphereNormal(dTheta,phi);
        sphereVertex(dTheta,phi);
    }
    glEnd();

    int iLimit = nThetaDivisions;
    if (thetaMax == M_PI) iLimit--;

    for (int i = 1; i < iLimit; i++) {
        glBegin(GL_QUAD_STRIP);
        float theta = i*dTheta;
        for (int j = 0; j <= nPhiDivisions; j++) {
            float phi = j*dPhi;
            sphereNormal(theta,phi);
            sphereVertex(theta,phi);
            sphereNormal(theta+dTheta,phi);
            sphereVertex(theta+dTheta,phi);
        }
        glEnd();
    }

    if (thetaMax == M_PI) {
        glBegin(GL_TRIANGLE_FAN);
        sphereNormal(M_PI,0);
        sphereVertex(M_PI,0);
        for (int j = 0; j <= nPhiDivisions; j++) {
            float phi = j*dPhi;
            sphereNormal(M_PI - dTheta, phi);
            sphereVertex(M_PI - dTheta, phi);
        }
        glEnd();
    }
    else if (solid) {
        glBegin(GL_TRIANGLE_FAN);
        sphereNormal(M_PI,0);
        glm::vec3 baseCenter = center + axis*cos(thetaMax);
        glVertex3f(baseCenter[0], baseCenter[1], baseCenter[2]);
        for (int j = 0; j <= nPhiDivisions; j++) {
            float phi = j*dPhi;
            sphereVertex(thetaMax,phi);
        }
        glEnd();
    }
}

void GlutDraw::drawDomeShell(
    glm::vec3 center, glm::vec3 axis,
    float thetaMax, float radiusRatio,
    int nThetaDivisionsFull, int nPhiDivisions)
{
    float r0 = glm::length(axis);
    if (r0 == 0) return;
    float r1 = radiusRatio*r0;

    float rInner = fmin(r0, r1);
    float rOuter = fmax(r0, r1);

    drawDome(glm::vec3(0, 0, 0), rOuter*glm::normalize(axis), thetaMax, false, true, nThetaDivisionsFull, nPhiDivisions);
    drawDome(glm::vec3(0, 0, 0), rInner*glm::normalize(axis), thetaMax, false, false, nThetaDivisionsFull, nPhiDivisions);

    if (radiusRatio == 1) return;

    float thetaMaxClamped = Math::clamp(0.0f, thetaMax, M_PI);
    if (thetaMaxClamped == 0 || thetaMaxClamped == M_PI) return;

    float dPhi = 2 * M_PI / nPhiDivisions;
    float zInner = rInner*cos(thetaMax);
    float zOuter = rOuter*cos(thetaMax);

    glm::vec3 w = axisAngleAlignZtoVEC3(axis);
    float angle = glm::length(w);
    glm::vec3 wHat(0, 0, 1);
    if (angle > 0) wHat = w / angle;

    float normalTheta = M_PI / 2 + thetaMax;
    auto stripNormal = [&](const float& phi) {
        glm::vec3 dir(sin(normalTheta)*cos(phi), sin(normalTheta)*sin(phi), cos(normalTheta));
        glm::vec3 n = glm::rotate(dir, angle, wHat);
        glNormal3f(n[0], n[1], n[2]);
    };
    auto stripVertices = [&](const float& phi) {
        glm::vec3 dir(sin(thetaMax)*cos(phi), sin(thetaMax)*sin(phi), cos(thetaMax));
        glm::vec3 n = glm::rotate(dir, angle, wHat);
        glVertex3f(center[0] + rInner*n[0], center[1] + rInner*n[1], center[2] + rInner*n[2]);
        glVertex3f(center[0] + rOuter*n[0], center[1] + rOuter*n[1], center[2] + rOuter*n[2]);
    };

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= nPhiDivisions; i++) {
        float phi = i*dPhi;
        stripNormal(phi);
        stripVertices(phi);
    }
    glEnd();
}

void GlutDraw::drawCylinder(glm::vec3 center, glm::vec3 axis, float r, int n)
{
    float h = glm::length(axis);
    if (h == 0) return;
    glm::vec3 topNormal = axis / h;

    float dTheta = 2 * M_PI / n;

    glm::vec3 w = axisAngleAlignZtoVEC3(axis);
    float angle = glm::length(w);
    glm::vec3 wHat(0, 0, 1);
    if (angle > 0) wHat = w / angle;

    auto normal = [&](const float& theta) {
        glm::vec3 n(cos(theta), sin(theta), 0);
        n = glm::rotate(n, angle, wHat);
        glNormal3f(n[0], n[1], n[2]);
    };
    auto topVertex = [&](const float& theta) {
        glm::vec3 v(r*cos(theta), r*sin(theta), h);
        v = glm::rotate(v, angle, wHat);
        glVertex3f(center[0] + v[0], center[1] + v[1], center[2] + v[2]);
    };
    auto bottomVertex = [&](const float& theta) {
        glm::vec3 v(r*cos(theta), r*sin(theta), -h);
        v = glm::rotate(v, angle, wHat);
        glVertex3f(center[0] + v[0], center[1] + v[1], center[2] + v[2]);
    };

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) {
        float theta = i*dTheta;
        normal(theta);
        topVertex(theta);
        bottomVertex(theta);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-topNormal[0], -topNormal[1], -topNormal[2]);
    glVertex3f(center[0] - axis[0], center[1] - axis[1], center[2] - axis[2]);
    for (int i = 0; i <= n; i++) {
        bottomVertex(i*dTheta);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(topNormal[0], topNormal[1], topNormal[2]);
    glVertex3f(center[0] + axis[0], center[1] + axis[1], center[2] + axis[2]);
    for (int i = 0; i <= n; i++) {
        topVertex(i*dTheta);
    }
    glEnd();
}

void GlutDraw::drawParallelepiped(glm::vec3 center, glm::vec3 xAxis, glm::vec3 yAxis, glm::vec3 zAxisIn) {

    glm::vec3 zAxis;
    if (glm::dot(glm::cross(xAxis, yAxis), zAxisIn) < 0) {
        zAxis = -zAxisIn;
    }
    else {
        zAxis = zAxisIn;
    }

    glm::vec3 pt000 = center - xAxis - yAxis - zAxis;
    glm::vec3 pt001 = center - xAxis - yAxis + zAxis;
    glm::vec3 pt010 = center - xAxis + yAxis - zAxis;
    glm::vec3 pt011 = center - xAxis + yAxis + zAxis;
    glm::vec3 pt100 = center + xAxis - yAxis - zAxis;
    glm::vec3 pt101 = center + xAxis - yAxis + zAxis;
    glm::vec3 pt110 = center + xAxis + yAxis - zAxis;
    glm::vec3 pt111 = center + xAxis + yAxis + zAxis;

    glm::vec3 xNormal = glm::normalize(glm::cross(yAxis, zAxis));
    glm::vec3 yNormal = glm::normalize(glm::cross(zAxis, xAxis));
    glm::vec3 zNormal = glm::normalize(glm::cross(xAxis, yAxis));

    glBegin(GL_QUADS);

    glNormal3f(xNormal[0], xNormal[1], xNormal[2]);
    glVertex3f(pt100[0], pt100[1], pt100[2]);
    glVertex3f(pt101[0], pt101[1], pt101[2]);
    glVertex3f(pt111[0], pt111[1], pt111[2]);
    glVertex3f(pt110[0], pt110[1], pt110[2]);

    glNormal3f(-xNormal[0], -xNormal[1], -xNormal[2]);
    glVertex3f(pt000[0], pt000[1], pt000[2]);
    glVertex3f(pt001[0], pt001[1], pt001[2]);
    glVertex3f(pt011[0], pt011[1], pt011[2]);
    glVertex3f(pt010[0], pt010[1], pt010[2]);

    glNormal3f(yNormal[0], yNormal[1], yNormal[2]);
    glVertex3f(pt010[0], pt010[1], pt010[2]);
    glVertex3f(pt011[0], pt011[1], pt011[2]);
    glVertex3f(pt111[0], pt111[1], pt111[2]);
    glVertex3f(pt110[0], pt110[1], pt110[2]);

    glNormal3f(-yNormal[0], -yNormal[1], -yNormal[2]);
    glVertex3f(pt000[0], pt000[1], pt000[2]);
    glVertex3f(pt001[0], pt001[1], pt001[2]);
    glVertex3f(pt101[0], pt101[1], pt101[2]);
    glVertex3f(pt100[0], pt100[1], pt100[2]);

    glNormal3f(zNormal[0], zNormal[1], zNormal[2]);
    glVertex3f(pt001[0], pt001[1], pt001[2]);
    glVertex3f(pt011[0], pt011[1], pt011[2]);
    glVertex3f(pt111[0], pt111[1], pt111[2]);
    glVertex3f(pt101[0], pt101[1], pt101[2]);

    glNormal3f(-zNormal[0], -zNormal[1], -zNormal[2]);
    glVertex3f(pt000[0], pt000[1], pt000[2]);
    glVertex3f(pt010[0], pt010[1], pt010[2]);
    glVertex3f(pt110[0], pt110[1], pt110[2]);
    glVertex3f(pt100[0], pt100[1], pt100[2]);

    glEnd();
}


void GlutDraw::drawPyramid(glm::vec3 base, glm::vec3 baseToTip, glm::vec3 baseToFirst, int nFaces, bool mode) {

    float r = glm::length(baseToFirst);
    glm::vec3 y = baseToFirst / r;
    glm::vec3 z = glm::normalize(baseToTip - glm::dot(baseToTip, y)*y);

    glm::vec3 baseToTip_local = baseToTip;
    glm::vec3 rotAxis = Math::axisAngleAlignVECStoZY3(z, y);
    float rotAngle = glm::length(rotAxis);
    if (rotAngle > 0) {
        rotAxis /= rotAngle;
        baseToTip_local = glm::rotate(baseToTip, M_PI*rotAngle / 2, rotAxis);
    }

    glPushMatrix();
    glTranslatef(base[0], base[1], base[2]);
    if (rotAngle > 0) glRotatef(90 * rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);

    float dPhi = 2 * M_PI / nFaces;

    glBegin(GL_TRIANGLES);
    for (int face = 0; face < nFaces; face++) {
        float phi0 = dPhi*face;
        float phi1 = dPhi*(face + 1);
        if (mode) {
            phi0 += dPhi / 2;
            phi1 += dPhi / 2;
        }
        glm::vec3 pt0 = r*glm::vec3(-sin(phi0), cos(phi0), 0);
        glm::vec3 pt1 = r*glm::vec3(-sin(phi1), cos(phi1), 0);

        glm::vec3 normal = glm::normalize(glm::cross(pt0 - baseToTip_local, pt1 - baseToTip_local));
        if (baseToTip_local[2] < 0) normal = -normal; // special case of rotAngle = PI, since we don't perform the rotation in this case

        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(baseToTip_local[0], baseToTip_local[1], baseToTip_local[2]);
        glVertex3f(pt0[0], pt0[1], pt0[2]);
        glVertex3f(pt1[0], pt1[1], pt1[2]);

        normal = glm::vec3(0, 0, -1);
        if (baseToTip_local[2] < 0) normal = -normal;

        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(0, 0, 0);
        glVertex3f(pt0[0], pt0[1], pt0[2]);
        glVertex3f(pt1[0], pt1[1], pt1[2]);
    }
    glEnd();

    glPopMatrix();
}

void GlutDraw::drawDoublePyramid(glm::vec3 base, glm::vec3 baseToTip, glm::vec3 baseToFirst, int nFaces, bool mode) {

    float r = glm::length(baseToFirst);
    glm::vec3 y = baseToFirst / r;
    glm::vec3 z = glm::normalize(baseToTip - glm::dot(baseToTip, y)*y);

    glm::vec3 baseToTip_local = baseToTip;
    glm::vec3 rotAxis = Math::axisAngleAlignVECStoZY3(z, y);
    float rotAngle = glm::length(rotAxis);
    if (rotAngle > 0) {
        rotAxis /= rotAngle;
        baseToTip_local = glm::rotate(baseToTip, M_PI*rotAngle / 2, rotAxis);
    }

    glPushMatrix();
    glTranslatef(base[0], base[1], base[2]);
    if (rotAngle > 0) glRotatef(90 * rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);

    float dPhi = 2 * M_PI / nFaces;

    glBegin(GL_TRIANGLES);
    for (int face = 0; face < nFaces; face++) {
        float phi0 = dPhi*face;
        float phi1 = dPhi*(face + 1);
        if (mode) {
            phi0 += dPhi / 2;
            phi1 += dPhi / 2;
        }
        glm::vec3 pt0 = r*glm::vec3(-sin(phi0), cos(phi0), 0);
        glm::vec3 pt1 = r*glm::vec3(-sin(phi1), cos(phi1), 0);

        glm::vec3 normal = glm::normalize(glm::cross(pt0 - baseToTip_local, pt1 - baseToTip_local));
        if (baseToTip_local[2] < 0) normal = -normal; // special case of rotAngle = PI, since we don't perform the rotation in this case

        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(baseToTip_local[0], baseToTip_local[1], baseToTip_local[2]);
        glVertex3f(pt0[0], pt0[1], pt0[2]);
        glVertex3f(pt1[0], pt1[1], pt1[2]);

        glNormal3f(-normal[0], -normal[1], -normal[2]);
        glVertex3f(-baseToTip_local[0], -baseToTip_local[1], -baseToTip_local[2]);
        glVertex3f(-pt0[0], -pt0[1], -pt0[2]);
        glVertex3f(-pt1[0], -pt1[1], -pt1[2]);
    }
    glEnd();

    glPopMatrix();
}