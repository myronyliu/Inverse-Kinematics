#ifndef _ROTATION_H_
#define _ROTATION_H_

#include "stdafx.h"

struct AxisAngleRotation2 {
    glm::vec2 _axis;
    float _angle;
    AxisAngleRotation2() : _axis(glm::vec2(0, 0)), _angle(0) {}
    AxisAngleRotation2(const glm::vec2& axis, const float& angle);
    AxisAngleRotation2(const float& angle, const glm::vec2& axis);
    AxisAngleRotation2(const glm::vec3& w);
    AxisAngleRotation2(const glm::mat3& R);
    float theta() const { return _axis[0]; }
    float phi() const  { return _axis[1]; }
    void clamp();
    void clampPhi() { _axis[1] -= (2 * M_PI)*floor(_axis[1] / (2 * M_PI)); }
    void clampAngle() { _angle -= (2 * M_PI)*floor(_angle / (2 * M_PI)); }
    void randPerturbAngle(const float& dAngle) { _angle += dAngle*(2.0f*rand() / RAND_MAX - 1); }
    void randPerturbAxis(const float& dzArcLength, const float& dPolar);
    // consider the rotation vector as specifying the orientation of a coordinate system
    // perturb(...) reorients this coordinate system slightly by...
    // jiggling its z-axis uniformly in a small "hemispherical cap" with geodesic-radius dzArcLength
    // and after having done so, rotates the new x-axis and new y-axis about the new z-axis uniformly in range [-dPolar,dPolar]
    glm::vec3 axisAngleRotation3() const { return _angle*glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0])); }
    glm::vec3 axis3() const { return glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0])); }
    glm::mat3 rotationMatrix() const;
    void pushRotation() const { if (_angle != 0) glRotatef((180.0f / M_PI)*_angle, sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0])); }
    AxisAngleRotation2 operator-() const { return AxisAngleRotation2(_axis, -_angle); }

    bool operator ==(const AxisAngleRotation2& rhs) { return _axis[0] == rhs._axis[0] && _axis[1] == rhs._axis[1] && _angle == rhs._angle; }
    void print() const;
};
bool operator == (const AxisAngleRotation2&, const AxisAngleRotation2&);

#endif