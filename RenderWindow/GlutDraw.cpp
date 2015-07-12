#include "GlutDraw.h"
#include "Math.h"


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
    glm::vec3 w = glm::cross(glm::vec3(0, 0, 1), -baseNormal);
    float phi = glm::length(w);
    if (phi > 0) w /= phi;
    phi *= 90.0f;

    float dTheta = 2 * M_PI / n;

    glPushMatrix();
    glTranslatef(base[0], base[1], base[2]);
    glRotatef(phi, w[0], w[1], w[2]);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-baseNormal[0], -baseNormal[1], -baseNormal[2]);
    glVertex3f(0, 0, h);
    for (int i = 0; i <= n; i++) {
        glm::vec3 normal = glm::normalize(glm::vec3(cos(i*dTheta), sin(i*dTheta), h));
        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), 0);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(baseNormal[0], baseNormal[1], baseNormal[2]);
    glVertex3f(0, 0, 0);
    for (int i = 0; i <= n; i++) {
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), 0);
    }
    glEnd();

    glPopMatrix();
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

void GlutDraw::drawSphere(glm::vec3 center, float r, int n, int m)
{
    glPushMatrix();
    glTranslatef(center[0], center[1], center[2]);
    glutSolidSphere(r, n, m);
    glPopMatrix();
}

void GlutDraw::drawCylinder(glm::vec3 center, glm::vec3 halfAxis, float r, int n)
{
    float h = glm::length(halfAxis);
    glm::vec3 topNormal = halfAxis / h;
    glm::vec3 w = glm::cross(glm::vec3(0, 0, 1), topNormal);
    float phi = glm::length(w);
    if (phi > 0) w /= phi;
    phi *= 90.0f;

    float dTheta = 2 * M_PI / n;

    glPushMatrix();
    glTranslatef(center[0], center[1], center[2]);
    glRotatef(phi, w[0], w[1], w[2]);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) {
        glNormal3f(cos(i*dTheta), sin(i*dTheta), 0);
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), -h);
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), h);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-topNormal[0], -topNormal[1], -topNormal[2]);
    glVertex3f(0, 0, -h);
    for (int i = 0; i <= n; i++) {
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), -h);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(topNormal[0], topNormal[1], topNormal[2]);
    glVertex3f(0, 0, h);
    for (int i = 0; i <= n; i++) {
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), h);
    }
    glEnd();

    glPopMatrix();
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