#include "SkeletonComponents.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

Bone::Bone(std::vector<Socket*> sockets, std::vector<Joint*> joints) {
    for (auto socket : sockets) attach(socket);
    for (auto joint : joints) attach(joint);
}

std::set<Bone*> Bone::reachableBones(Connection* excluded) {

    std::set<Bone*> visitedBones({ this });
    std::vector<Bone*> boneStack;

    for (auto socket : sockets()) {
        if (socket == excluded) continue;
        Bone* opposingBone = socket->opposingBone();
        if (opposingBone == NULL) continue;
        if (visitedBones.find(opposingBone) == visitedBones.end()) {
            visitedBones.insert(opposingBone);
            boneStack.push_back(opposingBone);
        }
    }
    for (auto joint : joints()) {
        if (joint == excluded) continue;
        Bone* opposingBone = joint->opposingBone();
        if (opposingBone == NULL) continue;
        if (visitedBones.find(opposingBone) == visitedBones.end()) {
            visitedBones.insert(opposingBone);
            boneStack.push_back(opposingBone);
        }
    }

    while (boneStack.size() > 0) {
        Bone* bone = boneStack.back();
        boneStack.pop_back();

        for (auto socket : bone->sockets()) {
            Bone* opposingBone = socket->opposingBone();
            if (opposingBone == NULL) continue;
            if (visitedBones.find(opposingBone) == visitedBones.end()) {
                visitedBones.insert(opposingBone);
                boneStack.push_back(opposingBone);
            }
        }
        for (auto joint : bone->joints()) {
            Bone* opposingBone = joint->opposingBone();
            if (opposingBone == NULL) continue;
            if (visitedBones.find(opposingBone) == visitedBones.end()) {
                visitedBones.insert(opposingBone);
                boneStack.push_back(opposingBone);
            }
        }
    }

    return visitedBones;
}

Scene::Skeleton* Bone::addToSkeleton(Scene::Skeleton* skeleton, Connection* excluded) {
    if (skeleton == _skeleton) return skeleton;
    for (auto bone : reachableBones(excluded))
        bone->_skeleton = skeleton;
    return skeleton;
}

Joint* Bone::attach(Joint* joint) {
    if (joint == NULL) return NULL;
    Bone* oldAnchor = joint->bone();
    Bone* target = joint->opposingBone();
    if (oldAnchor != NULL) {
        oldAnchor->_joints.erase(joint);
    }
    if (target != NULL) {
        if (target->_skeleton != _skeleton) {
            if (_skeleton == NULL) {
                addToSkeleton(target->_skeleton);
            }
            else if (target->_skeleton == NULL) {
                target->addToSkeleton(_skeleton);
            }
            else {
                delete target->_skeleton;
                target->addToSkeleton(_skeleton);
            }
        }
    }
    _joints.insert(joint);
    joint->_bone = this;
    return joint;
}
void Bone::detach(Joint* joint) {
    if (joint == NULL) return;
    auto it = _joints.find(joint);
    if (it != _joints.end()) {
        _joints.erase(it);
        joint->_bone = NULL;
        Bone* target = joint->opposingBone();
        if (target != NULL) {
            std::set<Bone*> reachableBones_fromDetached = target->reachableBones();
            auto it2 = reachableBones_fromDetached.find(this);
            if (it2 == reachableBones_fromDetached.end()) {
                target->addToSkeleton(new Scene::Skeleton(reachableBones_fromDetached));
            }
        }
    }
}
Socket* Bone::attach(Socket* socket) {
    if (socket == NULL) return NULL;
    Bone* oldAnchor = socket->bone();
    Bone* target = socket->opposingBone();
    if (oldAnchor != NULL) {
        oldAnchor->_sockets.erase(socket);
    }
    if (target != NULL) {
        if (target->_skeleton != _skeleton) {
            if (_skeleton == NULL) {
                addToSkeleton(target->_skeleton);
            }
            else if (target->_skeleton == NULL) {
                target->addToSkeleton(_skeleton);
            }
            else {
                delete target->_skeleton;
                target->addToSkeleton(_skeleton);
            }
        }
    }
    _sockets.insert(socket);
    socket->_bone = this;
    return socket;
}
void Bone::detach(Socket* socket) {
    if (socket == NULL) return;
    auto it = _sockets.find(socket);
    if (it != _sockets.end()) {
        _sockets.erase(it);
        socket->_bone = NULL;
        Bone* target = socket->opposingBone();
        if (target != NULL) {
            std::set<Bone*> reachableBones_fromDetached = target->reachableBones();
            auto it2 = reachableBones_fromDetached.find(this);
            if (it2 == reachableBones_fromDetached.end()) {
                target->addToSkeleton(new Scene::Skeleton(reachableBones_fromDetached));
            }
        }
    }
}

std::map<Connection*, Bone*> Bone::connectionToBones() const {
    std::map<Connection*, Bone*> map;
    for (auto joint : _joints)
        map[joint] = joint->opposingBone();
    for (auto socket : _sockets)
        map[socket] = socket->opposingBone();
    return map;
}
std::map<Socket*, Bone*> Bone::socketToBones() const {
    std::map<Socket*, Bone*> map;
    for (auto socket : _sockets)
        map[socket] = socket->opposingBone();
    return map;
}
std::map<Joint*, Bone*> Bone::jointToBones() const {
    std::map<Joint*, Bone*> map;
    for (auto joint : _joints)
        map[joint] = joint->opposingBone();
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

    //GlutDraw::drawSphere(glm::vec3(0, 0, 0), glm::vec3(0, 0, scale));

    if (_sockets.size() + _joints.size() < 3) {
        std::vector<glm::vec3> pts;
        for (auto socket : _sockets)
            pts.push_back(socket->_translationFromBone);
        for (auto joint : _joints)
            pts.push_back(joint->_translationFromBone);
        if (pts.size() == 1) {
            GlutDraw::drawCone(glm::vec3(0, 0, 0), glm::length(pts[0]) / 2, pts[0]);
        }
        else {
            /*float d0 = glm::length(pts[0]);
            float d1 = glm::length(pts[1]);
            float coneAngle = M_PI / 6;
            float cosTwoAngle = glm::dot(glm::normalize(pts[0]), glm::normalize(pts[1]));
            float angle = acos(Math::clamp(-1.0f, cosTwoAngle, 1.0f)) / 2;
            glm::length(pts[0])*/
            GlutDraw::drawCone(glm::vec3(0, 0, 0), glm::length(pts[0]) / 2, pts[0]);
            GlutDraw::drawCone(glm::vec3(0, 0, 0), glm::length(pts[1]) / 2, pts[1]);
        }
    }
    else {
        std::vector<glm::vec3> translations({ glm::vec3(0, 0, 0) });
        for (auto joint : _joints)
            translations.push_back(joint->translationFromBone());
        for (auto socket : _sockets)
            translations.push_back(socket->translationFromBone());
        GlutDraw::drawExhaustiveTriangles(translations);
    }
}