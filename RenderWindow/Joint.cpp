#include "Joint.h"

glm::mat3 BallJoint::rotRelPivR() const {
    glm::vec3 armAxisZ_relPivot(sin(_direction[0])*cos(_direction[0]), sin(_direction[0])*sin(_direction[1]), cos(_direction[0]));
    glm::vec3 armAxisY_relPivot(-sin(_spin), cos(_spin), 0); // BEFORE aligning
    glm::vec3 wAlign = -axisAngleAlignZ3(armAxisZ_relPivot);
    float angle = glm::length(wAlign);
    if (angle>0) armAxisY_relPivot = glm::rotate(armAxisY_relPivot, angle, wAlign / angle);
    glm::vec3 armAxisX_relPivot = glm::cross(armAxisY_relPivot, armAxisZ_relPivot);
    return glm::mat3(armAxisX_relPivot, armAxisY_relPivot, armAxisZ_relPivot);
}

void BallJoint::setRotation(const AxisAngleRotation2& rotation) {
    _rotation = rotation;
}
void BallJoint::setRotation(const glm::vec3& w) {
    _rotation = AxisAngleRotation2(w);
}
void BallJoint::setRotation(const glm::mat3& R) {
    _rotation = AxisAngleRotation2(R);
}
void BallJoint::setRotationAxis(const glm::vec3& w) {
    _rotation._axis = glm::vec2(acos(w[2] / glm::length(w)), atan2(w[1], w[0]));
}
void BallJoint::setRotationAxis(const glm::vec2& axis) {
    _rotation._axis = axis;
}
void BallJoint::setRotationAngle(const float& angle) {
    _rotation._angle = angle;
}

void BallJoint::setPivotAxes(const glm::mat3& xyz) {
    glm::vec3 z = glm::normalize(xyz[2]);
    glm::vec3 y = glm::normalize(xyz[1] - glm::dot(xyz[1], z)*z);
    glm::vec3 x = glm::cross(y, z);
    _pivotAxes = glm::mat3(x, y, z);
    _rotation = AxisAngleRotation2(composeLocalTransforms(_pivotAxes, rotRelPivR()));
}
void BallJoint::setPivotAxes(const glm::vec3& zIn, const glm::vec3& yIn) {
    glm::vec3 z = glm::normalize(zIn);
    glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
    glm::vec3 x = glm::cross(y, z);
    _pivotAxes = glm::mat3(x, y, z);
    _rotation = AxisAngleRotation2(composeLocalTransforms(_pivotAxes, rotRelPivR()));
}
void BallJoint::setSpin(const float& spin) {
    _spin = spin;
    _rotation = AxisAngleRotation2(composeLocalTransforms(_pivotAxes, rotRelPivR()));
}
void BallJoint::setDirection(const glm::vec2& direction) {
    _direction = direction;
    _rotation = AxisAngleRotation2(composeLocalTransforms(_pivotAxes, rotRelPivR()));
}
void BallJoint::setDirection(const glm::vec3& direction) {
    float norm = glm::length(direction);
    _direction = glm::vec2(acos(_direction[2] / norm), atan2(direction[1], direction[0]));
    _rotation = AxisAngleRotation2(composeLocalTransforms(_pivotAxes, rotRelPivR()));
}