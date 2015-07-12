#include "BallJoint.h"

using namespace Math;

glm::mat3 BallJoint::rotationFromPivotR() const {
    glm::vec3 armAxisZ_relPivot(sin(_directionFromPivot[0])*cos(_directionFromPivot[0]), sin(_directionFromPivot[0])*sin(_directionFromPivot[1]), cos(_directionFromPivot[0]));
    glm::vec3 armAxisY_relPivot(-sin(_spin), cos(_spin), 0); // BEFORE aligning
    glm::vec3 wAlign = axisAngleAlignZtoVEC3(armAxisZ_relPivot);
    float angle = glm::length(wAlign);
    if (angle>0) armAxisY_relPivot = glm::rotate(armAxisY_relPivot, angle, wAlign / angle);
    glm::vec3 armAxisX_relPivot = glm::cross(armAxisY_relPivot, armAxisZ_relPivot);
    return glm::mat3(armAxisX_relPivot, armAxisY_relPivot, armAxisZ_relPivot);
}

void BallJoint::setRotation(const AxisAngleRotation2& rotation) {
    _rotation = rotation;
    glm::mat3 R = rotationR();
    glm::mat3 R0 = rotationToPivotR();
    glm::mat3 R1 = R0*R*glm::inverse(R0);
    setRotationFromPivot(R1);
    clamp();
}
void BallJoint::setRotationAxis(const glm::vec3& w) {
    _rotation._axis = glm::vec2(acos(w[2] / glm::length(w)), atan2(w[1], w[0]));
    setRotation(_rotation);
}
void BallJoint::setRotationAxis(const glm::vec2& axis) {
    _rotation._axis = axis;
    setRotation(_rotation);
}
void BallJoint::setRotationAngle(const float& angle) {
    _rotation._angle = angle;
    setRotation(_rotation);
}

void BallJoint::setPivotAxes(const glm::mat3& xyz) {
    glm::vec3 z = glm::normalize(xyz[2]);
    glm::vec3 y = glm::normalize(xyz[1] - glm::dot(xyz[1], z)*z);
    glm::vec3 x = glm::cross(y, z);
    glm::mat3 R0 = glm::mat3(x, y, z);
    _rotationToPivot = axisAngleRotation3(R0);
    _rotation = AxisAngleRotation2(composeLocalTransforms(R0, rotationFromPivotR()));
}
void BallJoint::setPivotAxes(const glm::vec3& zIn, const glm::vec3& yIn) {
    glm::vec3 z = glm::normalize(zIn);
    glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
    glm::vec3 x = glm::cross(y, z);
    glm::mat3 R0 = glm::mat3(x, y, z);
    _rotationToPivot = axisAngleRotation3(R0);
    _rotation = AxisAngleRotation2(composeLocalTransforms(R0, rotationFromPivotR()));
}
void BallJoint::setSpin(const float& spin) {
    _spin = spin;
    _rotation = composeLocalRotations(_rotationToPivot, rotationFromPivot2());
}
void BallJoint::setDirection(const glm::vec2& direction) {
    _directionFromPivot = direction;
    _rotation = composeLocalRotations(_rotationToPivot, rotationFromPivot2());
}
void BallJoint::setDirection(const glm::vec3& direction) {
    float norm = glm::length(direction);
    _directionFromPivot = glm::vec2(acos(_directionFromPivot[2] / norm), atan2(direction[1], direction[0]));
    _rotation = composeLocalRotations(_rotationToPivot, rotationFromPivot2());
}

void BallJoint::setRotationFromPivot(const AxisAngleRotation2&) {}
void BallJoint::setRotationFromPivot(const glm::vec3&) {}
void BallJoint::setRotationFromPivot(const glm::mat3& R) {
    glm::vec3 z = glm::normalize(R[2]);
    glm::vec3 y = glm::normalize(R[1] - glm::dot(R[1], z)*z);
    glm::vec3 x = glm::cross(y, z);

    _directionFromPivot = glm::vec2(acos(z[2]), atan2(z[1], z[0]));
    glm::vec3 v = (z)*y;
    _spin = atan2(v[1], v[0]);
}


void BallJoint::draw(const float& radius) const {

    //if (_previous != NULL) {

    float transDist = glm::length(_translation);
    if (transDist > 0) {
        glm::vec3 w = axisAngleAlignVECtoZ3(_translation);

        glPushMatrix();
        pushRotation3(w);

        GlutDraw::drawDoublePyramid(
            transDist*glm::vec3(0, 0, 1.0f / 2),
            transDist*glm::vec3(0, 0, 1.0f / 2),
            transDist*glm::vec3(0, 1.0f / 8, 0));
        GlutDraw::drawParallelepiped(
            transDist*glm::vec3(1.0f / 8, 0, 1.0f / 2),
            transDist*glm::vec3(0, 1.0f / 64, 0),
            transDist*glm::vec3(0, 0, 1.0f / 64),
            transDist*glm::vec3(1.0f / 8, 0, 0));
        GlutDraw::drawParallelepiped(
            transDist*glm::vec3(0, 1.0f / 8, 1.0f / 2),
            transDist*glm::vec3(0, 0, 1.0f / 64),
            transDist*glm::vec3(1.0f / 64, 0, 0),
            transDist*glm::vec3(0, 1.0f / 8, 0));

        glPopMatrix();
    }

    //}

    glPushMatrix();

    glTranslatef(_translation[0], _translation[1], _translation[2]);
    _rotation.pushRotation();
    GlutDraw::drawSphere(glm::vec3(0, 0, 0), radius);

    glPopMatrix();
}

void BallJoint::perturb() {
    _rotation.randPerturbAxis(M_PI / 1024, M_PI / 1024);
    _rotation.randPerturbAngle(M_PI / 1024);
    setRotation(_rotation);
}

void BallJoint::constrain() {
    _spin -= (2 * M_PI)*floor(_spin / (2 * M_PI));
    // TODO fill in the other constraints
}

void BallJoint::setParamsFreely(const std::vector<float>& params) {
    /*_directionFromPivot[0] = params[0];
    _directionFromPivot[1] = params[1];
    _spin = params[2];
    _rotation = composeLocalRotations(_rotationToPivot, rotationFromPivot2());*/

    _rotation._axis[0] = params[0];
    _rotation._axis[1] = params[1];
    _rotation._angle = params[2];
    clamp();
}