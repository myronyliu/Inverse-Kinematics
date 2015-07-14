#include "HalfBallJoint.h"
#include "GlutDraw.h"

using namespace Math;

glm::mat3 HalfBallJoint::halfJointRotationR() const {
    glm::mat3 R0 = matrixAlignZtoVEC(mainAxis3());
    glm::mat3 R1 = glm::mat3(
        glm::vec3(cos(_spin), sin(_spin), 0),
        glm::vec3(-sin(_spin), cos(_spin), 0),
        glm::vec3(0, 0, 1));
    return composeLocalTransforms(R0, R1);
}

void HalfBallJoint::draw(const float& radius) const {
}

void HalfBallJoint::perturbFreely() {
    float dArc = M_PI / 1024;
    float randPhi = 2 * M_PI*rand() / RAND_MAX;
    _spin += (rand() < RAND_MAX / 2) ? M_PI / 1024 : -M_PI / 1024;

    glm::vec3 dAxis = glm::vec3(sin(dArc)*cos(randPhi), sin(dArc)*cos(randPhi), cos(dArc));

    glm::mat3 R1 = matrixAlignZtoVEC(dAxis);
    glm::mat3 R = composeLocalTransforms(halfJointRotationR(), R1);
    glm::vec3 newAxis = glm::normalize(R[3]);

    _mainAxis[0] = acos(newAxis[2]);
    _mainAxis[1] = atan2(newAxis[1], newAxis[0]);
}

void HalfBallJoint::constrain() {
    float theta = mod(_mainAxis[0], 2 * M_PI);
    float phi = mod(_mainAxis[1], 2 * M_PI);
    float spin = mod(_spin, 2 * M_PI);
    if (theta > M_PI) {
        theta = 2 * M_PI - theta;
        phi = -phi;
    } // trivial clamping has been performed to prevent degeneracies
    // proceed to actual user-specified constraints
    if (_constraints.size() > 0) {
        // constrain THETA
        if (_constraints[0] != NULL && _constraints[1] != NULL) {
            _constraints[0] = Math::clamp(0.0f, mod(_constraints[0], 2 * M_PI), M_PI);
            _constraints[1] = Math::clamp(0.0f, mod(_constraints[1], 2 * M_PI), M_PI);
            if (_constraints[0] < _constraints[1]) {
                theta = Math::clamp(_constraints[0], theta, _constraints[1]);
            }
            else if (_constraints[0] == _constraints[1]) {
                theta = _constraints[0];
            }
            else { // this is a bad case of the lower bound exceeding the upper bound, so we modify _constraints appropriately as well
                theta = (_constraints[0] + _constraints[1]) / 2;
                _constraints[0] = theta;
                _constraints[1] = theta;
            }
        }
        else if (_constraints[0] != NULL) {
            _constraints[0] = Math::clamp(0.0f, mod(_constraints[0], 2 * M_PI), M_PI);
            if (theta < _constraints[0]) theta = _constraints[0];
        }
        else if (_constraints[1] != NULL) {
            _constraints[1] = Math::clamp(0.0f, mod(_constraints[1], 2 * M_PI), M_PI);
            if (theta > _constraints[0]) theta = _constraints[1];
        }
        // constrain PHI
        if (_constraints[2] != NULL && _constraints[3] != NULL) {
            _constraints[2] = mod(_constraints[2], 2 * M_PI);
            _constraints[3] = mod(_constraints[3], 2 * M_PI);
            if (_constraints[3] < _constraints[2]) {
                _constraints[3] += 2 * M_PI;
            }
            if (theta < _constraints[2]) theta += 2 * M_PI;
            theta = mod(Math::clamp(_constraints[2], theta, _constraints[3]), 2 * M_PI);
        }
        // constrain SPIN
        if (_constraints[4] != NULL && _constraints[5] != NULL) {
            _constraints[4] = mod(_constraints[4], 2 * M_PI);
            _constraints[5] = mod(_constraints[5], 2 * M_PI);
            if (_constraints[5] < _constraints[4]) {
                _constraints[5] += 2 * M_PI;
            }
            if (theta < _constraints[4]) theta += 2 * M_PI;
            theta = mod(Math::clamp(_constraints[4], theta, _constraints[5]), 2 * M_PI);
        }
    }
    // reassign the constrained values to the joint parameters
    _mainAxis[0] = theta;
    _mainAxis[1] = phi;
    _spin = spin;
}


std::map<int, float> HalfBallJoint::getParams() {
    std::map<int, float> params;

    if (_constraints.size() > 0) {
        // check whether THETA is a degree of freedom
        if (_constraints[0] == NULL || _constraints[1] == NULL) {
            params[0] = _mainAxis[0];
        }
        else if (_constraints[0] != NULL && _constraints[1] != NULL) {
            _constraints[0] = Math::clamp(0.0f, mod(_constraints[0], 2 * M_PI), M_PI);
            _constraints[1] = Math::clamp(0.0f, mod(_constraints[1], 2 * M_PI), M_PI);
            if (_constraints[0] < _constraints[1]) params[0] = _mainAxis[0];
        }
        else if (_constraints[0] != NULL) {
            _constraints[0] = Math::clamp(0.0f, mod(_constraints[0], 2 * M_PI), M_PI);
            if (_constraints[0]<M_PI) params[0] = _mainAxis[0];
        }
        else if (_constraints[1] != NULL) {
            _constraints[1] = Math::clamp(0.0f, mod(_constraints[1], 2 * M_PI), M_PI);
            if (_constraints[0]>0) params[0] = _mainAxis[0];
        }
        // ... PHI ...
        if (_constraints[2] == NULL || _constraints[3] == NULL) {
            params[1] = _mainAxis[1];
        }
        else {
            _constraints[2] = mod(_constraints[2], 2 * M_PI);
            _constraints[3] = mod(_constraints[3], 2 * M_PI);
            if (_constraints[3] != _constraints[2]) params[1] = _mainAxis[1];
        }
        // ... SPIN ...
        if (_constraints[4] == NULL || _constraints[5] == NULL) {
            params[2] = _spin;
        }
        else {
            _constraints[4] = mod(_constraints[4], 2 * M_PI);
            _constraints[5] = mod(_constraints[5], 2 * M_PI);
            if (_constraints[4] != _constraints[5]) params[2] = _spin;
        }
    }

    return params;
}
void HalfBallJoint::setParamsFreely(std::map<int,float> params) {
    if (params[0] != NULL) _mainAxis[0] = params[0];
    if (params[1] != NULL) _mainAxis[1] = params[1];
    if (params[2] != NULL) _mainAxis[2] = params[2];
}