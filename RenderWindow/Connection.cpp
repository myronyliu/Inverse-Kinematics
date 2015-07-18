#include "SkeletonComponents.h"

using namespace Scene;

Connection::Connection(const int& i, const float& scale, Bone* bone) {

    if (i == 0) {
        _translationFromBone = scale*glm::vec3(1, 0, 0);
        _rotationFromBone = AxisAngleRotation2(glm::vec2(M_PI / 2, M_PI / 2), M_PI / 2);
    }
    else if (i == 1) {
        _translationFromBone = scale*glm::vec3(-1, 0, 0);
        _rotationFromBone = AxisAngleRotation2(glm::vec2(M_PI / 2, M_PI / 2), -M_PI / 2);
    }
    else if (i == 2) {
        _translationFromBone = scale*glm::vec3(0, 1, 0);
        _rotationFromBone = AxisAngleRotation2(glm::vec2(M_PI / 2, 0), -M_PI / 2);
    }
    else if (i == 3) {
        _translationFromBone = scale*glm::vec3(0, -1, 0);
        _rotationFromBone = AxisAngleRotation2(glm::vec2(M_PI / 2, 0), M_PI / 2);
    }
    else if (i == 4) {
        _translationFromBone = scale*glm::vec3(0, 0, 1);
        _rotationFromBone = AxisAngleRotation2(glm::vec2(0, 0), 0);
    }
    else if (i == 5) {
        _translationFromBone = scale*glm::vec3(0, 0, -1);
        _rotationFromBone = AxisAngleRotation2(glm::vec2(M_PI / 2, M_PI / 2), M_PI);
    }
}

Bone* Connection::attach(Bone* bone) {
    if (bone == NULL) return bone;
    if (Socket* socket = dynamic_cast<Socket*>(this))
        bone->attach(socket);
    else if (Joint* joint = dynamic_cast<Joint*>(this))
        bone->attach(joint);
    return bone;
}
void Connection::dettach() {
    if (Socket* socket = dynamic_cast<Socket*>(this))
        _bone->detach(socket);
    else if (Joint* joint = dynamic_cast<Joint*>(this))
        _bone->detach(joint);
}


void Connection::draw(const float& scale) const {
    drawAnchor(scale);

    glPushMatrix();
    pushTranslation(_translationFromBone);
    pushRotation(_rotationFromBone);

    drawPivot(scale);

    glPopMatrix();
}