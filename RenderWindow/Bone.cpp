#include "SkeletonComponents.h"

using namespace std;
using namespace glm;
using namespace Math;

void Bone::draw(const float& scale) const {

    doDraw();

    for (auto joint : _joints) {
        joint->draw(0.1);
    }
}

void Bone::doDraw(const float& scale) const {

    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    GlutDraw::drawParallelepiped(glm::vec3(scale, 0, 0), glm::vec3(0, scale, 0) / 8.0f, glm::vec3(0, 0, scale) / 8.0f, glm::vec3(scale, 0, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    GlutDraw::drawParallelepiped(glm::vec3(0, scale, 0), glm::vec3(0, 0, scale) / 8.0f, glm::vec3(scale, 0, 0) / 8.0f, glm::vec3(0, scale, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    GlutDraw::drawParallelepiped(glm::vec3(0, 0, scale), glm::vec3(scale, 0, 0) / 8.0f, glm::vec3(0, scale, 0) / 8.0f, glm::vec3(0, 0, scale));

    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    GlutDraw::drawSphere(glm::vec3(0, 0, 0), glm::vec3(0, 0, scale));

    GLint depth;
    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
    std::cout << depth << std::endl;

    GLenum errorEnum = glGetError();
    if (errorEnum != GL_NO_ERROR) {
        //std::cout << gluErrorString(errorEnum) << std::endl;
        GLint depth;
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
        int j = 0;
    }
}