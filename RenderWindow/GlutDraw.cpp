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
    glm::vec3 w = glm::cross(glm::vec3(0, 0, 1), axis / h);
    float phi = glm::length(w);
    if (phi > 0) w /= phi;
    phi *= 90;

    float dTheta = 2 * M_PI / n;

    glPushMatrix();
    glTranslatef(base[0], base[1], base[2]);
    glRotatef(phi, w[0], w[1], w[2]);

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, h);
        for (int i = 0; i <= n; i++) {
            glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), 0);
        }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);
        for (int i = 0; i <= n; i++) {
            glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), 0);
        }
    glEnd();

    glPopMatrix();
}

//TODO PRIO: drawRectangle so that we can do buttons.
void GlutDraw::drawRectangle(float x, float y,
        float width, float height)
{
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
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
    glm::vec3 w = glm::cross(glm::vec3(0, 0, 1), halfAxis / h);
    float phi = glm::length(w);
    if (phi > 0) w /= phi;
    phi *= 90;

    float dTheta = 2 * M_PI / n;

    glPushMatrix();
    glTranslatef(center[0], center[1], center[2]);
    glRotatef(phi, w[0], w[1], w[2]);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) {
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), -h);
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), h);
    }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, -h);
    for (int i = 0; i <= n; i++) {
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), -h);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, h);
    for (int i = 0; i <= n; i++) {
        glVertex3f(r*cos(i*dTheta), r*sin(i*dTheta), h);
    }
    glEnd();
}
