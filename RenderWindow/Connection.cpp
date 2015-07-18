#include "BodyComponents.h"

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


Skeleton* Connection::skeleton() {
    if (_bone == NULL && opposingBone() == NULL) return NULL;
    else if (_bone != NULL) return _bone->_skeleton;
    else return opposingBone()->_skeleton;
}
Connection* Connection::opposingConnection() {
    if (Socket* socket = dynamic_cast<Socket*>(this))
        return socket->joint();
    else if (Joint* joint = dynamic_cast<Joint*>(this))
        return joint->socket();
    else
        return NULL;
}
Bone* Connection::opposingBone() {
    if (Socket* socket = dynamic_cast<Socket*>(this)) {
        if (socket->joint() == NULL) return NULL;
        else return socket->joint()->bone();
    }
    else if (Joint* joint = dynamic_cast<Joint*>(this)) {
        if (joint->socket() == NULL) return NULL;
        else return joint->socket()->bone();
    }
    else
        return NULL;
}
std::pair<Socket*, Joint*> Connection::socketJoint() {
    if (Socket* socket = dynamic_cast<Socket*>(this))
        return std::make_pair(socket, socket->joint());
    else if (Joint* joint = dynamic_cast<Joint*>(this))
        return std::make_pair(joint->socket(), joint);
    else
        return std::make_pair((Socket*)NULL, (Joint*)NULL);
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