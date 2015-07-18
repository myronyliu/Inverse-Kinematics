#include "BallSocketJoint.h"
#include "GlutDraw.h"

using namespace Math;
using namespace Scene;

void BallSocket::perturbParams(const float& scale) {
    float theta = _params[0];
    float phi = _params[1];
    float spin = _params[2];

    float dArc = scale*M_PI / 512;
    float randPhi = 2 * M_PI*rand() / RAND_MAX;
    spin += scale*((rand() < RAND_MAX / 2) ? M_PI : -M_PI) / 512;

    glm::vec3 dAxis = glm::vec3(sin(dArc)*cos(randPhi), sin(dArc)*cos(randPhi), cos(dArc));

    glm::mat3 R1 = matrixAlignZtoVEC(dAxis);
    glm::mat3 R = revertFromBasis(R1, _rotationToJoint.rotationMatrix())*_rotationToJoint.rotationMatrix();
    glm::vec3 newAxis = glm::normalize(R[2]);

    std::map<int, float> newParams;
    _params[0] = acos(newAxis[2]);                // axis_theta
    _params[1] = atan2(newAxis[1], newAxis[0]);   // axis_phi
    _params[2] = spin;
}

void BallSocket::constrainParams() {
    if (_constraints.size() == 0) return;

    float lower, upper;

    float theta = mod(_params[0], 2 * M_PI);
    float phi = mod(_params[1], 2 * M_PI);
    float spin = mod(_params[2], 2 * M_PI);
    if (theta > M_PI) {
        theta = 2 * M_PI - theta;
        phi = -phi;
    } // trivial clamping has been performed to prevent degeneracies
    // proceed to actual user-specified constraints

    // constrain THETA
    if (getConstraint(0, lower) && getConstraint(1, upper))
        theta = clamp(fmax(0, lower), theta, fmin(M_PI, upper));
    else if (getConstraint(0, lower))
        theta = clamp(fmax(0, lower), theta, M_PI);
    else if (getConstraint(1, upper))
        theta = clamp(0.0f, theta, fmin(M_PI, upper));

    // constrain PHI
    if (getConstraint(2, lower) && getConstraint(3, upper))
        phi = periodicClamp(lower, phi, upper, 2 * M_PI);

    // constrain SPIN
    if (getConstraint(4, lower) && getConstraint(5, upper))
        spin = periodicClamp(lower, spin, upper, 2 * M_PI);

    // reassign the constrained values to the joint parameters
    _params[0] = theta;
    _params[1] = phi;
    _params[2] = spin;
}


std::map<int, float> BallSocket::adjustableParams() const {

    if (_constraints.size() == 0) return _params;

    float lower, upper;
    std::map<int, float> params;

    // check whether THETA is a degree of freedom
    if (!getConstraint(0, lower) && !getConstraint(1, upper))
        params[0] = _params.at(0);
    else if (getConstraint(0, lower) && getConstraint(1, upper)) {
        lower = Math::clamp(0.0f, mod(lower, 2 * M_PI), M_PI);
        upper = Math::clamp(0.0f, mod(upper, 2 * M_PI), M_PI);
        if (lower < upper) params[0] = _params.at(0);
    }
    else if (getConstraint(0, lower)) {
        lower = Math::clamp(0.0f, mod(lower, 2 * M_PI), M_PI);
        if (lower < M_PI) params[0] = _params.at(0);
    }
    else if (getConstraint(1, upper)) {
        upper = Math::clamp(0.0f, mod(upper, 2 * M_PI), M_PI);
        if (upper > 0) params[0] = _params.at(0);
    }

    // ... PHI ...
    if (!getConstraint(2, lower) || !getConstraint(3, upper))
        params[1] = _params.at(1);
    else {
        lower = mod(lower, 2 * M_PI);
        upper = mod(upper, 2 * M_PI);
        if (lower != upper) params[1] = params.at(1);
    }

    // ... SPIN ...
    if (!getConstraint(4, lower) || !getConstraint(5, upper))
        params[2] = params.at(2);
    else {
        lower = mod(lower, 2 * M_PI);
        upper = mod(upper, 2 * M_PI);
        if (lower != upper) params[2] = _params.at(2);
    }

    return params;
}

void BallSocket::buildParamsFromTransforms() {
    AxisSpinRotation axisSpin(_rotationToJoint);
    _params[0] = axisSpin._axis[0];
    _params[1] = axisSpin._axis[1];
    _params[2] = axisSpin._spin;
}

void BallJoint::drawPivot(const float& radius) const {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    GlutDraw::drawParallelepiped(glm::vec3(radius, 0, 0), glm::vec3(0, radius, 0) / 16.0f, glm::vec3(0, 0, radius) / 16.0f, glm::vec3(radius, 0, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    GlutDraw::drawParallelepiped(glm::vec3(0, radius, 0), glm::vec3(0, 0, radius) / 16.0f, glm::vec3(radius, 0, 0) / 16.0f, glm::vec3(0, radius, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    GlutDraw::drawParallelepiped(glm::vec3(0, 0, radius), glm::vec3(radius, 0, 0) / 16.0f, glm::vec3(0, radius, 0) / 16.0f, glm::vec3(0, 0, radius));

    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    GlutDraw::drawSphere(glm::vec3(0, 0, 0), glm::vec3(0, 0, radius));
}

void BallSocket::drawPivot(const float& radius) const {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    GlutDraw::drawParallelepiped(glm::vec3(radius, 0, 0), glm::vec3(0, radius, 0) / 16.0f, glm::vec3(0, 0, radius) / 16.0f, glm::vec3(radius, 0, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    GlutDraw::drawParallelepiped(glm::vec3(0, radius, 0), glm::vec3(0, 0, radius) / 16.0f, glm::vec3(radius, 0, 0) / 16.0f, glm::vec3(0, radius, 0));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    GlutDraw::drawParallelepiped(glm::vec3(0, 0, radius), glm::vec3(radius, 0, 0) / 16.0f, glm::vec3(0, radius, 0) / 16.0f, glm::vec3(0, 0, radius));

    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    /*GlutDraw::drawDomeShell(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1.1*radius), M_PI/2, 1.1f);
    return;*/

    if (_constraints.size() == 0) return;

    float thetaMin, thetaMax, phiMin, phiMax, spinMin, spinMax;

    if (getConstraint(0, thetaMin) && thetaMin > 0)
        GlutDraw::drawDomeShell(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.1*radius), thetaMin, 1.1f);
    if (getConstraint(1, thetaMax) && thetaMax < M_PI)
        GlutDraw::drawDomeShell(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1.1*radius), M_PI - thetaMax, 1.1f);

    if (getConstraint(2, phiMin) && getConstraint(3, phiMax)) {
        phiMin = mod(phiMin, 2 * M_PI);
        phiMax = mod(phiMax, 2 * M_PI);
        if (phiMax < phiMin)phiMax += 2 * M_PI;
        float phi = (phiMin + phiMax) / 2;
        GlutDraw::drawWedgeShell(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.1*1.1*radius), -glm::vec3(cos(phi), sin(phi), 0), 2 * M_PI - (phiMax - phiMin), 1.1f);
    }
}