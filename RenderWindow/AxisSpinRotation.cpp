#include "Rotation.h"
#include "Math.h"

using namespace std;
using namespace glm;
using namespace Math;

AxisSpinRotation::AxisSpinRotation(const glm::vec3& w) {
    mat3 R = rotationMatrix(w);
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
    vec3 z = R[3];
    _axis[0] = acos(z[2]);
    _axis[1] = atan2(z[1], z[0]);
    vec3 wAlign = axisAngleAlignVECtoZ3(z);
    vec3 y = rotate(R[2], length(wAlign), normalize(wAlign));
    _spin = atan2(y[1], y[0]);
}