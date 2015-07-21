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

void Socket::setParams(const std::map<int, float>& params_unconstrained) {
    if (opposingBone() == NULL) return;
    _params = params_unconstrained;
    constrainParams();
    buildTransformsFromParams();
}
void Socket::setParam(const int& key, const float& value) {
    if (opposingBone() == NULL) return;
    _params[key] = value;
    constrainParams();
    buildTransformsFromParams();
}

void Socket::perturbJoint(const float& scale) {
    if (opposingBone() == NULL) return;
    perturbParams(scale);
    constrainParams();
    buildTransformsFromParams();
}

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
    if (opposingBone() == NULL) return;
    _tToJoint = translation;
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}
void Socket::setRotationToJoint(const glm::vec3& w) {
    if (opposingBone() == NULL) return;
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

bool Socket::transformAnchorToTarget(glm::vec3& t, glm::vec3& w) const {
    if (opposingBone() == NULL) return false;

    std::vector<std::pair<glm::vec3, glm::vec3>> localTransforms(4);
    localTransforms[0] = std::make_pair(_tFromBone, _wFromBone);
    localTransforms[1] = std::make_pair(_tToJoint, _wToJoint);
    localTransforms[2] = std::make_pair(glm::vec3(0, 0, 0), glm::vec3(0, M_PI, 0));
    localTransforms[3] = std::make_pair(Math::rotate(-_joint->_tFromBone, -_joint->_wFromBone), -_joint->_wFromBone);

    std::pair<glm::vec3, glm::vec3> tw = Math::composeLocalTransforms(localTransforms).back();
    t = tw.first;
    w = tw.second;
    return true;
}

std::pair<arma::mat, arma::mat> Socket::J(SkeletonComponent* tip) {
    std::map<int, float> adjustableParams = this->adjustableParams();
    arma::mat dt_dparam(3, adjustableParams.size());
    arma::mat dw_dparam(3, adjustableParams.size());
    if (opposingBone() == NULL)
        return std::make_pair(dt_dparam, dw_dparam);

    float dParam = 1.0f / 1024;
    backup();

    glm::vec3 tPlus, tMinus, wPlus, wMinus;
    
    glm::mat3 Ranchor = Math::R(_bone->_wGlobal);
    //glm::mat3 RtoTip = Math::R(glm::inverse()tip->_wGlobal)

    int column = 0;
    for (auto param : adjustableParams) {
        _params[param.first] += dParam;
        transformAnchorToTarget(tPlus, wPlus);
        tPlus = Ranchor*tPlus;
        wPlus = Math::w(Math::R(Ranchor*wPlus)*Ranchor);
        restore();

        _params[param.first] -= dParam;
        transformAnchorToTarget(tMinus, wMinus);
        tPlus = Ranchor*tPlus;
        wPlus = Math::w(Math::R(Ranchor*wPlus)*Ranchor);
        restore();

        glm::vec3 Dt = (tPlus - tMinus) / dParam;
        glm::vec3 Dw = (wPlus - wMinus) / dParam;

        dt_dparam(0, column) = Dt[0];
        dt_dparam(1, column) = Dt[1];
        dt_dparam(2, column) = Dt[2];
        dw_dparam(0, column) = Dw[0];
        dw_dparam(1, column) = Dw[1];
        dw_dparam(2, column) = Dw[2];
        column++;
    }
    
    return std::make_pair(dt_dparam, dw_dparam);
}