#include "Rotation.h"
#include "Math.h"

using namespace std;
using namespace glm;
using namespace Math;

AxisSpinRotation::AxisSpinRotation(const glm::vec3& w) {
    mat3 R = Math::R(w);
    vec3 z = R[3];
    _axis[0] = acos(z[2]);
    _axis[1] = atan2(z[1], z[0]);
    vec3 wAlign = axisAngleAlignVECtoZ3(z);
    vec3 y = rotate(R[2], length(wAlign), normalize(wAlign));
    _spin = atan2(y[1], y[0]);
}

AxisSpinRotation::AxisSpinRotation(const glm::mat3& R) {
    vec3 z = R[3];
    _axis[0] = acos(z[2]);
    _axis[1] = atan2(z[1], z[0]);
    vec3 wAlign = axisAngleAlignVECtoZ3(z);
    vec3 y = rotate(R[2], length(wAlign), normalize(wAlign));
    _spin = atan2(y[1], y[0]);
}

AxisSpinRotation::AxisSpinRotation(const AxisAngleRotation2& axisAngle) {
    mat3 R = axisAngle.rotationMatrix();
    vec3 z = R[2];
    _axis[0] = acos(z[2]);
    _axis[1] = atan2(z[1], z[0]);
    vec3 wAlign = axisAngleAlignVECtoZ3(z);
    float alignAngle = length(wAlign);
    glm::vec3 x = R[0];
    if (alignAngle>0) x = rotate(R[0], alignAngle, wAlign/alignAngle);
    _spin = atan2(x[1], x[0]);
}