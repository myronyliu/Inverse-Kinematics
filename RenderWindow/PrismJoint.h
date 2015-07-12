#ifndef _PRISMJOINT_H_
#define _PRISMJOINT_H_

#include "Joint.h"

class PrismJoint : public Joint
{
public:
    PrismJoint() :
        Joint(), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}
    PrismJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        Joint(translation, rotation), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}
    PrismJoint(const glm::vec3& translation, const glm::vec3& w) :
        Joint(translation, w), _rotationToPivot(AxisAngleRotation2()), _directionFromPivot(glm::vec2(0, 0)), _spin(0) {}
    PrismJoint(const glm::vec3& translation, const glm::mat3& R) :
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

    void setRotation(const AxisAngleRotation2&);
    void setRotation(const glm::vec3& w) { setRotation(AxisAngleRotation2(w)); }
    void setRotation(const glm::mat3& R) { setRotation(AxisAngleRotation2(R)); }
    void setRotationAxis(const glm::vec3&);
    void setRotationAxis(const glm::vec2& axis);
    void setRotationTheta(const float& theta) { _rotation._axis[0] = theta; setRotationAxis(_rotation._axis); }
    void setRotationPhi(const float& phi) { _rotation._axis[1] = phi; setRotationAxis(_rotation._axis); }
    void setRotationAngle(const float& angle);


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
    std::vector<float> getParams() const {
        //return std::vector<float>({ _directionFromPivot[0], _directionFromPivot[1], _spin });
        return std::vector<float>({ _rotation._axis[0], _rotation._axis[1], _rotation._angle });
    }
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