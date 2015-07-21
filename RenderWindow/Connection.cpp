#include "BodyComponents.h"

using namespace Math;
using namespace Scene;

Connection::Connection(const int& i, const float& scale, Bone* bone) : SkeletonComponent()
{
    if (i == 0) {
        _tFromBone = scale*glm::vec3(1, 0, 0);
        _wFromBone = glm::vec3(0, M_PI / 2, 0);
    }
    else if (i == 1) {
        _tFromBone = scale*glm::vec3(-1, 0, 0);
        _wFromBone = glm::vec3(0, -M_PI / 2, 0);
    }
    else if (i == 2) {
        _tFromBone = scale*glm::vec3(0, 1, 0);
        _wFromBone = glm::vec3(-M_PI / 2, 0, 0);
    }
    else if (i == 3) {
        _tFromBone = scale*glm::vec3(0, -1, 0);
        _wFromBone = glm::vec3(M_PI / 2, 0, 0);
    }
    else if (i == 4) {
        _tFromBone = scale*glm::vec3(0, 0, 1);
        _wFromBone = glm::vec3(0, 0, 0);
    }
    else if (i == 5) {
        _tFromBone = scale*glm::vec3(0, 0, -1);
        _wFromBone = glm::vec3(0, M_PI, 0);
    }
}


Skeleton* Connection::skeleton() const {
    if (_bone == NULL && opposingBone() == NULL) return NULL;
    else if (_bone != NULL) return _bone->_skeleton;
    else return opposingBone()->_skeleton;
}
Connection* Connection::opposingConnection() const {
    if (const Socket* socket = dynamic_cast<const Socket*>(this))
        return socket->joint();
    else if (const Joint* joint = dynamic_cast<const Joint*>(this))
        return joint->socket();
    else
        return NULL;
}
Bone* Connection::opposingBone() const {
    if (const Socket* socket = dynamic_cast<const Socket*>(this)) {
        if (socket->joint() == NULL) return NULL;
        else return socket->joint()->bone();
    }
    else if (const Joint* joint = dynamic_cast<const Joint*>(this)) {
        if (joint->socket() == NULL) return NULL;
        else return joint->socket()->bone();
    }
    else
        return NULL;
}

glm::vec3 Connection::translationToOpposingConnection() const {
    if (const Socket* socket = dynamic_cast<const Socket*>(this))
        return socket->translationToJoint();
    else if (const Joint* joint = dynamic_cast<const Joint*>(this)) {
        return joint->socket()->translationFromJoint();
    }
    else
        return glm::vec3(0, 0, 0);
}
glm::vec3 Connection::rotationToOpposingConnection() const {
    if (const Socket* socket = dynamic_cast<const Socket*>(this))
        return socket->rotationToJoint();
    else if (const Joint* joint = dynamic_cast<const Joint*>(this))
        return joint->socket()->rotationFromJoint();
    else
        return glm::vec3(0, 0, 0);
}
glm::vec3 Connection::translationFromOpposingConnection() const {
    if (const Socket* socket = dynamic_cast<const Socket*>(this))
        return socket->translationFromJoint();
    else if (const Joint* joint = dynamic_cast<const Joint*>(this)) {
        return joint->socket()->translationToJoint();
    }
    else
        return glm::vec3(0, 0, 0);
}
glm::vec3 Connection::rotationFromOpposingConnection() const {
    if (const Socket* socket = dynamic_cast<const Socket*>(this))
        return socket->rotationFromJoint();
    else if (const Joint* joint = dynamic_cast<const Joint*>(this))
        return joint->socket()->rotationToJoint();
    else
        return glm::vec3(0, 0, 0);
}

bool Connection::alignToConnection(Connection* target, glm::vec3& t, glm::vec3& w) {

    Connection* opposingConnection = this->opposingConnection();
    if (opposingConnection == target) {
        t = translationToOpposingConnection();
        w = rotationToOpposingConnection();
        return true;
    }

    Bone* opposingBone = opposingConnection->bone();

    if (_bone == NULL && opposingBone == NULL)
        return false;

    Bone* root;
    std::vector<std::pair<glm::vec3, glm::vec3>> localTransforms;
    if (_bone != NULL) {
        localTransforms.push_back(std::make_pair(-_tFromBone, -_wFromBone));
        root = _bone;
    }
    else {
        localTransforms.push_back(std::make_pair(translationToOpposingConnection(), rotationToOpposingConnection()));
        localTransforms.push_back(std::make_pair(-opposingConnection->_tFromBone, -opposingConnection->_wFromBone));
        root = opposingBone;
    }

    // Begin Graph Search

    std::vector<std::pair<Bone*, std::vector<Connection*>>> stack;
    stack.push_back(std::make_pair(root, std::vector<Connection*>()));
    std::set<Bone*> visited;

    bool floatingTarget = target->_bone == NULL;
    Bone* bone;
    std::vector<Connection*> path;
    do {
        std::pair<Bone*, std::vector<Connection*>> bonePath = stack.back();
        bone = bonePath.first;
        path = bonePath.second;
        stack.pop_back();

        if (bone->hasConnection(target)) break;

        for (auto connection : bone->connections()) {
            Bone* opposingBone = connection->opposingBone();
            if (opposingBone == NULL) {
                if (!floatingTarget) continue;
                else if (connection->opposingConnection() == target) {
                    path.push_back(connection);
                    break;
                }
                else continue;
            }
            if (visited.find(opposingBone) != visited.end()) continue;
            visited.insert(opposingBone);
            std::vector<Connection*> extendedPath = path;
            extendedPath.push_back(connection);
            stack.push_back(std::make_pair(opposingBone, extendedPath));
        }
    } while (stack.size() > 0);

    if (stack.size() == 0) return false;

    glm::vec3 translation, rotation;
    for (int i = 0; i < path.size() - 1; i++) {
        path[i]->transformAnchorToTarget(translation, rotation);
        localTransforms.push_back(std::make_pair(translation, rotation));
    }
    if (path.back()->transformAnchorToTarget(translation, rotation)) { // for "non-floating" target
        localTransforms.push_back(std::make_pair(translation, rotation));
        localTransforms.push_back(std::make_pair(target->_tFromBone, target->_wFromBone));
    }
    else {
        Connection* lastConnection = path.back();
        localTransforms.push_back(std::make_pair(lastConnection->_tFromBone, lastConnection->_wFromBone));
        localTransforms.push_back(std::make_pair(
            lastConnection->translationToOpposingConnection(),
            lastConnection->rotationToOpposingConnection()));
    }
    auto tw = Math::composeLocalTransforms(localTransforms).back();
    t = tw.first;
    w = tw.second;
    return true;
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
    pushTranslation(_tFromBone);
    pushRotation(_wFromBone);

    drawPivot(scale);

    glPopMatrix();
}

TreeNode<Connection*>* Connection::connectionTree() {
    /*if (opposingConnection() == NULL) return new TreeNode<Connection*>(NULL);
    
    Connection* node = this;
    TreeNode<Connection*>* root = new TreeNode<Connection*>(node);

    Bone* opposingBone = this->opposingBone();
    if (opposingBone == NULL) return root;

    std::vector<std::tuple<Bone*, Connection*, TreeNode<Connection*>*>> stack;
    stack.push_back(std::make_tuple(opposingBone, opposingConnection(), root));
    std::set<Bone*> visitedBones({ opposingBone });

    Bone* bone;
    Connection* connectionToPrevious;
    TreeNode<Connection*>* parent;
    do {
        std::tie(bone, connectionToPrevious, parent) = stack.back();
        stack.pop_back();

        for (auto connection : bone->connections()) {
            if (connection == connectionToPrevious) continue;
            TreeNode<Connection*>* subtree = new TreeNode<Connection*>(connection, parent);
            opposingBone = connection->opposingBone();
            if (opposingBone == NULL || visitedBones.find(opposingBone) == visitedBones.end()) continue;
            visitedBones.insert(opposingBone);
            stack.push_back(std::make_tuple(opposingBone, connection->opposingConnection(), subtree));
        }
    } while (stack.size() > 0);

    return root;*/
    return NULL;
}

TreeNode<Bone*>* Connection::boneTree() {
    if (_bone == NULL) return NULL;

    Bone* node = _bone;
    TreeNode<Bone*>* root = new TreeNode<Bone*>(_bone);

    Bone* opposingBone = this->opposingBone();
    if (opposingBone == NULL) return root;

    std::vector<std::tuple<Bone*, Connection*, TreeNode<Bone*>*>> stack;
    stack.push_back(std::make_tuple(opposingBone, opposingConnection(), root));
    std::set<Bone*> visitedBones({ opposingBone });

    Bone* bone;
    Connection* connectionToPrevious;
    TreeNode<Bone*>* parent;
    do {
        std::tie(bone, connectionToPrevious, parent) = stack.back();
        stack.pop_back();

        for (auto connection : bone->connections()) {
            if (connection == connectionToPrevious) continue;
            TreeNode<Bone*>* subtree = new TreeNode<Bone*>(bone, parent);
            opposingBone = connection->opposingBone();
            if (opposingBone == NULL || visitedBones.find(opposingBone) != visitedBones.end()) continue;
            visitedBones.insert(opposingBone);
            stack.push_back(std::make_tuple(opposingBone, connection->opposingConnection(), subtree));
        }
    } while (stack.size() > 0);

    return root;
    return NULL;
}