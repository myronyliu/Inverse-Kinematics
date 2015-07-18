#include "BodyComponents.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;


bool Bone::insertConnection(Connection* connection) {
    if (connection == NULL) return true;
    if (Socket* socket = dynamic_cast<Socket*>(connection)) {
        _sockets.insert(socket);
        return true;
    }
    else if (Joint* joint = dynamic_cast<Joint*>(connection)) {
        _joints.insert(joint);
        return true;
    }
    return false;
}
bool Bone::eraseConnection(Connection* connection) {
    if (connection == NULL) return true;
    if (Socket* socket = dynamic_cast<Socket*>(connection)) {
        _sockets.erase(socket);
        return true;
    }
    else if (Joint* joint = dynamic_cast<Joint*>(connection)) {
        _joints.erase(joint);
        return true;
    }
    return false;
}

Bone::Bone(vector<Socket*> sockets, vector<Joint*> joints) {
    for (auto socket : sockets) attach(socket);
    for (auto joint : joints) attach(joint);
}

map<Bone*,vector<Connection*>> Bone::reachableBonePaths(Connection* excluded) {

    map<Bone*, vector<Connection*>> visited({ make_pair(this, vector<Connection*>(0)) });
    vector<pair<Bone*, vector<Connection*>>> stack;

    for (auto connection : connections()) {
        if (connection == excluded) continue;
        Bone* opposingBone = connection->opposingBone();
        if (opposingBone == NULL) continue;
        if (visited.find(opposingBone) == visited.end()) {
            stack.push_back(make_pair(opposingBone, vector<Connection*>({ connection })));
            visited.insert(stack.back());
        }
    }

    while (stack.size() > 0) {
        Bone* bone = stack.back().first;
        vector<Connection*> path = stack.back().second;
        // I don't think it's safe to "std::tie" here, since we pop_back immediately after
        stack.pop_back();

        for (auto connection : bone->connections()) {
            Bone* opposingBone = connection->opposingBone();
            if (opposingBone == NULL) continue;
            if (visited.find(opposingBone) == visited.end()) {
                vector<Connection*> extendedPath = path;
                extendedPath.push_back(connection);
                stack.push_back(make_pair(opposingBone, extendedPath));
                visited.insert(stack.back());
            }
        }
    }

    return visited;
}
set<Bone*> Bone::reachableBones(Connection* excluded) {
    set<Bone*> set;
    map<Bone*, vector<Connection*>> map = reachableBonePaths();
    for (auto pair : map) { set.insert(pair.first); }
    return set;
}

Scene::Skeleton* Bone::addToSkeleton(Scene::Skeleton* skeleton, Connection* excluded) {
    if (skeleton == _skeleton) return skeleton;
    for (auto bone : reachableBones(excluded)) {
        bone->_skeleton = skeleton;
        skeleton->_bones.insert(bone);
    }
    return skeleton;
}

Joint* Bone::attach(Joint* joint) {
    if (joint == NULL)
        return joint;
    else if (_joints.find(joint) != _joints.end())
        return joint;

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
            set<Bone*> reachableBones_fromDetached = target->reachableBones();
            auto it2 = reachableBones_fromDetached.find(this);
            if (it2 == reachableBones_fromDetached.end()) {
                target->addToSkeleton(new Scene::Skeleton(*reachableBones_fromDetached.begin()));
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
            set<Bone*> reachableBones_fromDetached = target->reachableBones();
            auto it2 = reachableBones_fromDetached.find(this);
            if (it2 == reachableBones_fromDetached.end()) {
                target->addToSkeleton(new Scene::Skeleton(*reachableBones_fromDetached.begin()));
            }
        }
    }
}

set<Connection*> Bone::connections() const {
    set<Connection*> connections(_sockets.begin(), _sockets.end());
    for (auto joint : _joints) { connections.insert(joint); }
    return connections;
}
map<Connection*, Bone*> Bone::connectionToBones() const {
    map<Connection*, Bone*> map;
    for (auto joint : _joints)
        map[joint] = joint->opposingBone();
    for (auto socket : _sockets)
        map[socket] = socket->opposingBone();
    return map;
}
map<Socket*, Bone*> Bone::socketToBones() const {
    map<Socket*, Bone*> map;
    for (auto socket : _sockets)
        map[socket] = socket->opposingBone();
    return map;
}
map<Joint*, Bone*> Bone::jointToBones() const {
    map<Joint*, Bone*> map;
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
        vector<glm::vec3> pts;
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
        vector<glm::vec3> translations({ glm::vec3(0, 0, 0) });
        for (auto joint : _joints)
            translations.push_back(joint->translationFromBone());
        for (auto socket : _sockets)
            translations.push_back(socket->translationFromBone());
        GlutDraw::drawExhaustiveTriangles(translations);
    }
}