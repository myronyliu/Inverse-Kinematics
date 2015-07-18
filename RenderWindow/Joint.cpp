#include "BodyComponents.h"
#include "GlutDraw.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

Socket* Joint::couple(Socket* socket) {
    if (socket == _socket || socket->type() != type())
        return socket;
    if (socket == NULL)
        decouple();
    else {
        if (_bone != NULL) {
            _bone->_joints.erase(this);
            _bone->attach(this);
        }
        socket->_joint = this;
        _socket = socket;
    }
    return socket;
}
void Joint::decouple() {
    if (_socket != NULL) {
        if (_bone != NULL) {
            Bone* bone = _bone;                 // 1: Backup the bone to which this socket is anchored
            _bone->detach(this);                // 2: Detach this socket from the anchor (keeping the socket-joint link in tact)
            bone->_joints.insert(this);         // 3: Reattach this socket to its anchor without skeleton updates
        }
        _socket->_joint = NULL;                 // 4: Sever the socket-joint link
        _socket = NULL;                         //    ...
    }
}

std::pair<glm::vec3, AxisAngleRotation2> Joint::alignAnchorToTarget() {
    if (opposingBone() == NULL)
        return make_pair(glm::vec3(0, 0, 0), AxisAngleRotation2(glm::vec2(0, 0), 0));

    glm::vec3 translation;
    AxisAngleRotation2 rotation;
    tie(translation, rotation) = _socket->alignAnchorToTarget();

    return make_pair((-rotation).rotationMatrix()*(-translation), -rotation);
}

std::pair<glm::vec3, AxisAngleRotation2> Joint::alignThisToBone() {
    return make_pair(glm::vec3(0, 0, 0), AxisAngleRotation2(glm::vec2(0, 0), 0));
}
