#include "GlutDraw.h"

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
        glNormal3f(baseNormal[0],baseNormal[1],baseNormal[2]);
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
