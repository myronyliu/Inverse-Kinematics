#include "BodyComponents.h"
#include "GlutDraw.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

//////////////////////
//// CONSTRUCTORS ////
//////////////////////

Socket::Socket(const int& i, const float& scale, Bone* bone) :
Connection(i, scale, NULL), _tToJoint(glm::vec3(0, 0, 0)), _wToJoint(glm::vec3(0, 0, 0))
{
    attach(bone);
}

/////////////////
//// GETTERS ////
/////////////////

bool Socket::getConstraint(const int& key, float& value) const {
    auto it = _constraints.find(key);
    if (it == _constraints.end())
        return false;
    else {
        value = it->second;
        return true;
    }
}
bool Socket::getParam(const int& key, float& value) const {
    auto it = _params.find(key);
    if (it == _params.end())
        return false;
    else {
        value = it->second;
        return true;
    }
}

/////////////////
//// SETTERS ////
/////////////////

Joint* Socket::couple(Joint* joint) {
    if (joint == _joint || joint->type() != type())
        return joint;
    if (joint == NULL)
        decouple();
    else {
        if (_bone != NULL) {
            _bone->_sockets.erase(this);
            _bone->attach(this);
        }
        joint->_socket = this;
        _joint = joint;
    }
    return joint;
}
void Socket::decouple() {
    if (_joint != NULL) {
        if (_bone != NULL) {
            Bone* bone = _bone;                 // 1: Backup the bone to which this socket is anchored
            _bone->detach(this);                // 2: Detach this socket from the anchor (keeping the socket-joint link in tact)
            bone->_sockets.insert(this);        // 3: Reattach this socket to its anchor without skeleton updates
        }
        _joint->_socket = NULL;                 // 4: Sever the socket-joint link
        _joint = NULL;                          //    ...
    }
}

void Socket::setTranslationToJoint(const glm::vec3& translation) {
    _tToJoint = translation;
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}
void Socket::setRotationToJoint(const glm::vec3& w) {
    _wToJoint = Math::clampRotation(w);
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}

void Socket::setConstraint(const int& key, const float& value) {
    _constraints[key] = value;
    constrainParams();
    buildTransformsFromParams();
}

std::pair<glm::vec3, glm::vec3> Socket::alignAnchorToTarget() {
    if (opposingBone() == NULL)
        return make_pair(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));

    glm::mat3 SocketAxes_Socketed = Math::R(_wFromBone);
    glm::mat3 SocketToJoint_Socketed = revertFromBasis(Math::R(_wToJoint), SocketAxes_Socketed);
    glm::mat3 FlippedJointAxes_Socketed = SocketToJoint_Socketed*SocketAxes_Socketed;
    glm::mat3 JointAxes_Socketed = FlippedJointAxes_Socketed;
    JointAxes_Socketed[0] *= -1;
    JointAxes_Socketed[2] *= -1;
    // ^ z axes face opposite directions and y's are aligned (our arbitrary convention)

    glm::vec3 t = _tFromBone + FlippedJointAxes_Socketed*(_tToJoint);

    glm::mat3 JointAxes_Jointed = Math::R(_joint->_wFromBone);
    glm::mat3 JointedAxes_Joint = glm::inverse(JointAxes_Jointed);
    
    // The following aligns the joint axes from the Jointed Bone frame to the
    // joint axes in the Socketed Bone frame
    glm::mat3 R_align = JointAxes_Socketed*JointedAxes_Joint;
    t -= R_align*_joint->_tFromBone;

    auto asdf = R_align*_joint->_tFromBone;

    glm::mat3 JointToJointed_Socketed
        = revertFromBasis(JointedAxes_Joint, JointAxes_Socketed);
    // we've now reversed from the (unflipped) JointAxes representation of the last rotation, to the Socketed-Body representation

    glm::mat3 SocketedToJointed_Socketed = JointToJointed_Socketed*JointAxes_Socketed;

    return make_pair(t, Math::w(SocketedToJointed_Socketed));
}

std::pair<glm::vec3, glm::vec3> Socket::alignThisToBone() {
    return make_pair(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
}

arma::mat Socket::Jacobian_ParamsToTip(Connection*) {

    return arma::mat();

    float delta = 1.0f / 1024;

    backup();

    std::map<int, float> adjustable = adjustableParams();
    for (auto param : adjustable) {
        _params[param.first] -= delta;

    }
    

    restore();
}