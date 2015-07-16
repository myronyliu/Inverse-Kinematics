#include "Rotation.h"
#include "Math.h"
#include "utils.h"

using namespace std;
using namespace glm;
using namespace Math;

bool operator == (const AxisAngleRotation2& lhs, const AxisAngleRotation2& rhs) {
    return lhs._axis[0] == rhs._axis[0] && lhs._axis[1] == rhs._axis[1] && lhs._angle == rhs._angle;
}

AxisAngleRotation2::AxisAngleRotation2(const glm::mat3& R) {
    AxisAngleRotation2 axisAngle = Math::axisAngleRotation2(R);
    _axis = axisAngle._axis;
    _angle = axisAngle._angle;
}
AxisAngleRotation2::AxisAngleRotation2(const glm::vec2& axis, const float& angleIn) {
    float theta = axis[0];
    float phi = axis[1];
    float angle = angleIn;
    angle -= (2 * M_PI)*floor(angle / (2 * M_PI));

    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta > M_PI) {
        theta = 2 * M_PI - theta;
        phi += M_PI;
    }
    phi -= (2 * M_PI)*floor(phi / (2 * M_PI));
    _axis[0] = theta;
    _axis[1] = phi;
    _angle = angle;
}
AxisAngleRotation2::AxisAngleRotation2(const float& angleIn, const glm::vec2& axis) {
    float theta = axis[0];
    float phi = axis[1];
    float angle = angleIn;
    angle -= (2 * M_PI)*floor(angle / (2 * M_PI));

    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta > M_PI) {
        theta = 2 * M_PI - theta;
        phi += M_PI;
    }
    phi -= (2 * M_PI)*floor(phi / (2 * M_PI));
    _axis[0] = theta;
    _axis[1] = phi;
    _angle = angle;
}
AxisAngleRotation2::AxisAngleRotation2(const glm::vec3& w) {
    float angle = glm::length(w);
    if (angle == 0) {
        _axis = glm::vec2(0, 0);
        _angle = 0;
    }
    else {
        _axis[0] = acos(w[2] / angle);
        _axis[1] = atan2(w[1], w[0]);
        _angle = angle - (2 * M_PI)*floor(angle / (2 * M_PI));
    }
}
AxisAngleRotation2::AxisAngleRotation2(const AxisSpinRotation& axisSpin) {

    float theta = axisSpin._axis[0];
    float phi = axisSpin._axis[1];
    float spin = axisSpin._spin;

    glm::vec3 x(cos(spin), sin(spin), 0);
    glm::vec3 y(-sin(spin), cos(spin), 0);
    glm::vec3 z(0, 0, 1);

    glm::vec3 v(cos(M_PI / 2 + phi), sin(M_PI / 2 + phi), 0);

    x = glm::rotate(x, theta, v);
    y = glm::rotate(y, theta, v);
    z = glm::rotate(z, theta, v);

    glm::vec3 w = Math::axisAngleRotation3(glm::mat3(x, y, z));

    _angle = glm::length(w);
    if (_angle == 0)
        _axis = glm::vec2(0, 0);
    else {
        _axis[0] = acos(w[2] / _angle);
        _axis[1] = atan2(w[1], w[0]);
    }
}



glm::vec3 AxisAngleRotation2::axisAngleRotation3() const {
    return Math::mod(_angle, 2 * M_PI)*glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
}

glm::mat3 AxisAngleRotation2::rotationMatrix() const {
    glm::vec3 wHat = glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    float clampedAngle = _angle - (2 * M_PI)*floor(_angle / (2 * M_PI));
    if (fabs(clampedAngle - M_PI) < 0.000001) {
        return glm::mat3() + 2.0f * wCross*wCross;
    }
    else {
        return glm::mat3() + sin(_angle)*wCross + (1 - cos(_angle))*wCross*wCross;
    }
}
void AxisAngleRotation2::clamp() {
    _angle -= (2 * M_PI)*floor(_angle / (2 * M_PI));

    _axis[0] -= (2 * M_PI)*floor(_axis[0] / (2 * M_PI));
    if (_axis[0] > M_PI) {
        _axis[0] = 2 * M_PI - _axis[0];
        _axis[1] += M_PI;
    }
    _axis[1] -= (2 * M_PI)*floor(_axis[1] / (2 * M_PI));
}
void AxisAngleRotation2::randPerturbAxis(const float& dzArcLength, const float& dPolar) {

    float randTheta = acos(1 - (1 - cos(dzArcLength))*rand() / RAND_MAX);
    float randPhi = 2 * M_PI*rand() / RAND_MAX;
    float randPolar = dPolar * (2.0f*rand() / RAND_MAX - 1);

    glm::mat3 axes = rotationMatrix();
    glm::vec3 x = axes[0];
    glm::vec3 y = axes[1];
    glm::vec3 z = axes[2];

    glm::vec3 wAlignWorld;
    glm::vec3 zTargetLocal;
    glm::vec3 wAlignTarget;
    if (z[0] <= z[1] && z[0] <= z[2]) {
        Math::axisAngleAlignFirstToSecond(z, glm::vec3(1, 0, 0));
        zTargetLocal = glm::vec3(cos(randTheta), sin(randTheta)*cos(randPhi), sin(randTheta)*sin(randPhi));
        wAlignTarget = Math::axisAngleAlignFirstToSecond(glm::vec3(1, 0, 0), zTargetLocal);
    }
    else if (z[1] <= z[2] && z[1] <= z[0]) {
        Math::axisAngleAlignFirstToSecond(z, glm::vec3(0, 1, 0));
        zTargetLocal = glm::vec3(sin(randTheta)*sin(randPhi), cos(randTheta), sin(randTheta)*cos(randPhi));
        wAlignTarget = Math::axisAngleAlignFirstToSecond(glm::vec3(0, 1, 0), zTargetLocal);
    }
    else {
        Math::axisAngleAlignFirstToSecond(z, glm::vec3(0, 0, 1));
        zTargetLocal = glm::vec3(sin(randTheta)*cos(randPhi), sin(randTheta)*sin(randPhi), cos(randTheta));
        wAlignTarget = Math::axisAngleAlignFirstToSecond(glm::vec3(0, 0, 1), zTargetLocal);
    }
    float alignWorldAngle = glm::length(wAlignWorld);

    // rotate axes to world-axes
    if (alignWorldAngle > 0) {
        glm::vec3 rotAxis = wAlignWorld / alignWorldAngle;
        x = glm::rotate(x, alignWorldAngle, rotAxis);
        y = glm::rotate(y, alignWorldAngle, rotAxis);
        z = glm::rotate(z, alignWorldAngle, rotAxis);
    }

    // rotate axes such that z and zTargetLocal are aligned
    if (fabs(randTheta) > 0) {
        glm::vec3 rotAxis = wAlignTarget / randTheta;
        x = glm::rotate(x, fabs(randTheta), rotAxis);
        y = glm::rotate(y, fabs(randTheta), rotAxis);
        z = glm::rotate(z, fabs(randTheta), rotAxis);
    }

    // rotate x,y-axes about the z-axis
    x = glm::rotate(x, randPolar, z);
    y = glm::rotate(y, randPolar, z);

    // undo the first rotation (note that the first glm::rotate conceptually captures TWO rotations)
    if (alignWorldAngle > 0) {
        glm::vec3 rotAxis = wAlignWorld / alignWorldAngle;
        x = glm::rotate(x, -alignWorldAngle, rotAxis);
        y = glm::rotate(y, -alignWorldAngle, rotAxis);
        z = glm::rotate(z, -alignWorldAngle, rotAxis);
    }

    glm::mat3 new_R = glm::mat3(x, y, z);
    glm::vec3 new_axis = glm::normalize(Math::axisAngleRotation3(new_R));
    if (glm::dot(axis3(), new_axis) < 0) new_axis = -new_axis;

    _axis[0] = acos(new_axis[2]);
    _axis[1] = atan2(new_axis[1], new_axis[0]);

    // No need to reset the angle, since any changes are purely from numerical error
}

void AxisAngleRotation2::print() const {
    printf("( ");
    printVec2(_axis, false);
    if (_angle<0) printf(" )  %f\n", _angle);
    else printf(" )   %f\n", _angle);
}