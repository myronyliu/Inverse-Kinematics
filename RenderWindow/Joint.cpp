#include "SkeletonComponents.h"
#include "GlutDraw.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

Bone* Joint::attach(Bone* bone) {
    if (bone == _bone)
        return bone;
    if (bone != NULL)
        bone->_joints.insert(this);
    if (_bone != NULL)
        _bone->_joints.erase(this);
    _bone = bone;
    return bone;
}
void Joint::dettach() {
    if (_bone == NULL) return;
    _bone->_joints.erase(this);
    _bone = NULL;
}

Bone* Joint::opposingBone() const {
    if (_socket == NULL) return NULL;
    else return _socket->bone();
}

Socket* Joint::couple(Socket* socket) {
    if (socket == _socket || socket->type() != type())
        return socket;

    if (socket == NULL) {
        decouple();
    }
    else {
        socket->_joint = NULL;
        _socket = socket;
    }
    return socket;
}
void Joint::decouple() {
    if (_socket != NULL) {
        _socket->_joint = NULL;
    }
    _socket = NULL;
}

std::pair<glm::vec3, AxisAngleRotation2> Joint::alignAnchorToTarget() const {
    if (opposingBone() == NULL)
        return make_pair(glm::vec3(0, 0, 0), AxisAngleRotation2(glm::vec2(0, 0), 0));

    glm::vec3 translation;
    AxisAngleRotation2 rotation;
    tie(translation, rotation) = _socket->alignAnchorToTarget();

    return make_pair((-rotation).rotationMatrix()*(-translation), -rotation);
}
