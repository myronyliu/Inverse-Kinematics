#ifndef _JOINT_H_
#define _JOINT_H_

#include "stdafx.h"
#include "utils.h"
#include "math.h"

// TODO: add Track-Joint class (similar to PRISM except with translation along a specified path)

// NOTE: try to limit all "clamping of _rotation" to root Joint class
class Joint
{
public:
    Joint() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2(glm::vec2(0, 0), 0)) {}
    Joint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        _translation(translation), _rotation(rotation) {}
    Joint(const glm::vec3& translation, const glm::vec3& w) :
        _translation(translation), _rotation(AxisAngleRotation2(w)) {}
    Joint(const glm::vec3& translation, const glm::mat3& R) :
        _translation(translation), _rotation(AxisAngleRotation2(R)) {}

    void clamp() {
        _rotation.clamp();
        constrain();
        _rotation.clamp();
    }

    // FUNCTIONS THAT MUST BE DEFINED IN CHILDREN CLASSES
    virtual void constrain() {};
    std::vector<float> getParams() const { return std::vector<float>(0); };
    virtual void setParamsFreely(const std::vector<float>&) {};
    virtual void perturb() {};
    virtual void draw(const float&) const {};
    virtual void backupExtras() {};
    virtual void restoreExtras() {};

    void setParams(const std::vector<float>& params) {
        setParamsFreely(params);
        clamp();
    }
    void restore() {
        _rotation = _rotation_stash;
        _translation = _translation_stash;
        restoreExtras();
    }
    void backup() {
        _rotation_stash = _rotation;
        _translation_stash = _translation;
        backupExtras();
    }


    std::pair<glm::vec3, AxisAngleRotation2> tryParams(const std::vector<float>& params) {
        Joint copy = *this;
        copy.setParams(params);
        return std::pair<glm::vec3, AxisAngleRotation2>(copy.translation(), copy.rotation2());
    }

    int nParams() const { return getParams().size(); }
    
    // The following are virtual, because sometimes they affect the inherited class's member variables as well
    // ... in which case, we need to redefine these functions
    virtual void setTranslation(const glm::vec3& translation) { _translation = translation; }
    virtual void setRotation(const AxisAngleRotation2& rotation) { _rotation = rotation; }
    virtual void setRotation(const glm::vec3& w) { _rotation = AxisAngleRotation2(w); }
    virtual void setRotation(const glm::mat3& R) { _rotation = AxisAngleRotation2(R); }
    virtual void setRotationAxis(const glm::vec3& w) { _rotation._axis = glm::vec2(acos(w[2] / glm::length(w)), atan2(w[1], w[0])); }
    virtual void setRotationAxis(const glm::vec2& axis) { _rotation._axis = axis; }
    virtual void setRotationTheta(const float& theta) { _rotation._axis[0] = theta; }
    virtual void setRotationPhi(const float& phi) { _rotation._axis[1] = phi; }
    virtual void setRotationAngle(const float& angle) { _rotation._angle = angle; }

    glm::vec3 translation() const { return _translation; }
    AxisAngleRotation2 rotation2() const { return _rotation; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationR() const { return _rotation.rotationMatrix(); }

protected:
    glm::vec3 _translation; // relative to the previous axes
    AxisAngleRotation2 _rotation; // relative to the current axes

    // STASHES for backup and restore
    glm::vec3 _translation_stash;
    AxisAngleRotation2 _rotation_stash;
};

class BallJoint : public Joint
{
public:
    BallJoint() :
        Joint(), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}
    BallJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        Joint(translation, rotation), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}
    BallJoint(const glm::vec3& translation, const glm::vec3& w) :
        Joint(translation, w), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}
    BallJoint(const glm::vec3& translation, const glm::mat3& R) :
        Joint(translation, R), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}

    // SETTERS
    void setPivotAxes(const glm::mat3&);
    void setPivotAxes(const glm::vec3& z, const glm::vec3& y);
    void setSpin(const float&);
    void setDirection(const glm::vec2&);
    void setDirection(const glm::vec3&);

    void setRotationFromPivot(const AxisAngleRotation2&);
    void setRotationFromPivot(const glm::vec3&);
    void setRotationFromPivot(const glm::mat3&);

    virtual void setRotation(const AxisAngleRotation2&);
    virtual void setRotation(const glm::vec3& w) { setRotation(AxisAngleRotation2(w)); }
    virtual void setRotation(const glm::mat3& R) { setRotation(AxisAngleRotation2(R)); }
    virtual void setRotationAxis(const glm::vec3&);
    virtual void setRotationAxis(const glm::vec2& axis);
    virtual void setRotationTheta(const float& theta) { _rotation._axis[0] = theta; setRotationAxis(_rotation._axis); }
    virtual void setRotationPhi(const float& phi) { _rotation._axis[1] = phi; setRotationAxis(_rotation._axis); }
    virtual void setRotationAngle(const float& angle);


    glm::mat3 pivotAxes() const { return _rotationToPivot.rotationMatrix(); }
    glm::mat3 rotationToPivotR() const { return _rotationToPivot.rotationMatrix(); } // same as previous function (Aliased)
    glm::vec3 rotationToPivot3() const { return _rotationToPivot.axisAngleRotation3(); }
    AxisAngleRotation2 rotationToPivot2() const { return _rotationToPivot; }

    glm::mat3 rotationFromPivotR() const;
    glm::vec3 rotationFromPivot3() const { return Math::axisAngleRotation3(rotationFromPivotR()); }
    AxisAngleRotation2 rotationFromPivot2() const { return Math::axisAngleRotation2(rotationFromPivotR()); }

    void perturb();
    void constrain();
    void setParamsFreely(const std::vector<float>&);
    std::vector<float> getParams() const { return std::vector<float>({ _directionFromPivot[0], _directionFromPivot[1], _spin }); }
    virtual void draw(const float&) const;
    void backupExtras() {
        _rotationToPivot_stash = _rotationToPivot;
        _directionFromPivot_stash = _directionFromPivot;
        _spin_stash = _spin;
    }
    void restoreExtras() {
        _rotationToPivot = _rotationToPivot_stash;
        _directionFromPivot = _directionFromPivot_stash;
        _spin = _spin_stash;
    }


private:
    // For _spin=0 and _direction=(0,0), the segment points along _rotationToPivot.z with "up-direction" _rotationToPivot.y
    // For _spin=pi/2 and _direction=(pi/2,pi/2), the segment points along _rotationToPivot.y with "up-direction" _rotationToPivot.z
    // That is... _direction orients the segment (by rotating it away from _rotationToPivot.z in the most direct manner)
    //            _spin rotates the segment about its own reoriented main axis
    // We have defined the ball joint parameters in this way since the most natural constraints are in this context
    // e.g. constraining _direction.theta from [0,pi/2) restricts the segment's main-axis to the upper hemisphere (as defined by _rotationToPivot.z)
    AxisAngleRotation2 _rotationToPivot;
    glm::vec2 _directionFromPivot;
    float _spin; // We have intentionally avoided using AxisAngleRotation2 here, since [_direction,_spin] do not define a rotation in the usual sense

    // STASHES
    AxisAngleRotation2 _rotationToPivot_stash;
    glm::vec2 _directionFromPivot_stash;
    float _spin_stash;
};

#endif