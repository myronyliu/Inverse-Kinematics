#ifndef _HALFBALLJOINT_H_
#define _HALFBALLJOINT_H_

#include "stdafx.h"
#include "SkeletonComponents.h"
#include "Rotation.h"
#include "Math.h"

class HalfBallJoint : public HalfJoint
{
public:
    HalfBallJoint() :
        HalfJoint(), _mainAxis(glm::vec2(0,0)), _spin(0) {}
    HalfBallJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        HalfJoint(translation, rotation), _mainAxis(glm::vec2(0, 0)), _spin(0) {}
    HalfBallJoint(const glm::vec3& translation, const glm::vec3& w) :
        HalfJoint(translation, w), _mainAxis(glm::vec2(0, 0)), _spin(0) {}
    HalfBallJoint(const glm::vec3& translation, const glm::mat3& R) :
        HalfJoint(translation, R), _mainAxis(glm::vec2(0, 0)), _spin(0) {}

    /////////////////
    //// SETTERS ////
    /////////////////

    void setSpin(const float& spin) { _spin = spin; }
    void setMainAxis(const glm::vec2& thetaPhi) { _mainAxis = thetaPhi; }
    void setMainAxis(const glm::vec3& v) { _mainAxis[0] = acos(v[2] / glm::length(v)); _mainAxis[1] = atan2(v[1], v[0]); }

    /////////////////
    //// GETTERS ////
    /////////////////

    float spin() const { return _spin; }
    glm::vec2 mainAxis2() const { return _mainAxis; }
    glm::vec3 mainAxis3() const {
        float theta = _mainAxis[0];
        float phi = _mainAxis[1];
        return glm::vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
    }

    ////////////////////////////
    //// DERIVED QUANTITIES ////
    ////////////////////////////

    glm::vec3 halfJointTranslation() const { return glm::vec3(0,0,0); }
    glm::mat3 halfJointRotationR() const;
    glm::vec3 halfJointRotation3() const { return Math::axisAngleRotation3(halfJointRotationR()); }
    AxisAngleRotation2 halfJointRotation2() const { return AxisAngleRotation2(halfJointRotationR()); }

    void perturbFreely();
    void constrain();
    void setParamsFreely(std::map<int, float>);
    std::map<int, float> getParams();
    virtual void draw(const float&) const;

    void backup() { _mainAxis_stash = _mainAxis; _spin_stash = _spin; }
    void restore() { _mainAxis = _mainAxis_stash; _spin = _spin_stash; }


private:
    glm::vec2 _mainAxis;
    float _spin;

    // STASHES
    glm::vec2 _mainAxis_stash;
    float _spin_stash;
};


#endif