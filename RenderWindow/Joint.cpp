#include "SkeletonComponents.h"
#include "GlutDraw.h"

using namespace std;
using namespace glm;
using namespace Math;

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