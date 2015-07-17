#include "SkeletonComponents.h"

using namespace std;
using namespace glm;
using namespace Math;

Bone::Bone(std::vector<Socket*> sockets, std::vector<Joint*> joints) {
    for (auto socket : sockets) attach(socket);
    for (auto joint : joints) attach(joint);
}

Joint* Bone::attach(Joint* joint) {
    if (joint == NULL) return NULL;
    if (joint->bone() != NULL)
        joint->bone()->_joints.erase(joint);
    _joints.insert(joint);
    return joint;
}
void Bone::detach(Joint* joint) {
    if (joint == NULL) return;
    auto it = _joints.find(joint);
    if (it != _joints.end()) {
        _joints.erase(it);
        joint->_bone = NULL;
    }
}
Socket* Bone::attach(Socket* socket) {
    if (socket == NULL) return NULL;
    if (socket->bone() != NULL)
        socket->bone()->_sockets.erase(socket);
    _sockets.insert(socket);
    return socket;
}
void Bone::detach(Socket* socket) {
    if (socket == NULL) return;
    auto it = _sockets.find(socket);
    if (it != _sockets.end()) {
        _sockets.erase(it);
        socket->_bone = NULL;
    }
}

std::map<Connection*, Bone*> Bone::connectionToBones() const {
    std::map<Connection*, Bone*> map;
    for (auto joint : _joints)
        map[joint] = joint->socket()->bone();
    for (auto socket : _sockets)
        map[socket] = socket->joint()->bone();
    return map;
}
std::map<Socket*, Bone*> Bone::socketToBones() const {
    std::map<Socket*, Bone*> map;
    for (auto socket : _sockets)
        map[socket] = socket->joint()->bone();
    return map;
}
std::map<Joint*, Bone*> Bone::jointToBones() const {
    std::map<Joint*, Bone*> map;
    for (auto joint : _joints)
        map[joint] = joint->socket()->bone();
    return map;
}

Connection* Bone::getConnectionToBone(Bone* neighbor) const {
    if (neighbor == NULL) return NULL;
    for (auto joint : _joints)
        if (joint->socket()->bone() == neighbor) return joint;
    for (auto socket : _sockets)
        if (socket->joint()->bone() == neighbor) return socket;
    return NULL;
}

/////////////////
//// DRAWING ////
/////////////////

void Bone::draw(const float& scale) const {
    doDraw();

    for (auto socket : _sockets) {
        socket->draw(0.1);
    }
}

void Bone::doDraw(const float& scale) const {

    /*glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    GlutDraw::drawParallelepiped(glm::vec3(scale, 0, 0), glm::vec3(0, scale, 0) / 8.0f, glm::vec3(0, 0, scale) / 8.0f, glm::vec3(scale, 0, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    GlutDraw::drawParallelepiped(glm::vec3(0, scale, 0), glm::vec3(0, 0, scale) / 8.0f, glm::vec3(scale, 0, 0) / 8.0f, glm::vec3(0, scale, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    GlutDraw::drawParallelepiped(glm::vec3(0, 0, scale), glm::vec3(scale, 0, 0) / 8.0f, glm::vec3(0, scale, 0) / 8.0f, glm::vec3(0, 0, scale));

    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    GlutDraw::drawSphere(glm::vec3(0, 0, 0), glm::vec3(0, 0, scale));*/

    if (_sockets.size() + _joints.size() < 2)
        GlutDraw::drawSphere(glm::vec3(0, 0, 0), glm::vec3(0, 0, scale));
    else {
        std::vector<glm::vec3> translations({ glm::vec3(0, 0, 0) });
        for (auto joint : _joints)
            translations.push_back(joint->translationFromBone());
        for (auto socket : _sockets)
            translations.push_back(socket->translationFromBone());
        GlutDraw::drawExhaustiveTriangles(translations);
    }
}