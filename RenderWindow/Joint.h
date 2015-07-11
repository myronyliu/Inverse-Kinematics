#ifndef _JOINT_H_
#define _JOINT_H_

#include "stdafx.h"
#include "utils.h"

// TODO: add Track-Joint class (similar to PRISM except with translation along a specified path)

class Joint
{
public:
    Joint() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2(glm::vec2(0, 0), 0)), _constraints(std::map<int,float>()) {}
    Joint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        _translation(translation), _rotation(rotation), _constraints(std::map<int, float>()) {}
    Joint(const glm::vec3& translation, const glm::vec3& w) :
        _translation(translation), _rotation(AxisAngleRotation2(w)), _constraints(std::map<int, float>()) {}
    Joint(const glm::vec3& translation, const glm::mat3& R) :
        _translation(translation), _rotation(AxisAngleRotation2(R)), _constraints(std::map<int, float>()) {}

    virtual std::vector<float> getParams() const { return std::vector<float>(0); }
    virtual void setParams(const std::vector<float>& params) {}

    void setTranslation(const glm::vec3& translation) { _translation = translation; }
    void setRotation(const AxisAngleRotation2& rotation) { _rotation = rotation; }
    void setRotation(const glm::vec3& w) { _rotation = AxisAngleRotation2(w); }
    void setRotation(const glm::mat3& R) { _rotation = AxisAngleRotation2(R); }
    void setRotationAxis(const glm::vec3& w) { _rotation._axis = glm::vec2(acos(w[2] / glm::length(w)), atan2(w[1], w[0])); }
    void setRotationAxis(const glm::vec2& axis) { _rotation._axis = axis; }
    void setRotationAngle(const float& angle) { _rotation._angle = angle; }

    AxisAngleRotation2 rotation2() const { return _rotation; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationR() const { return _rotation.rotationMatrix(); }

    virtual void perturb() {}
    virtual void draw(const float& scale) const {}

protected:
    AxisAngleRotation2 _rotation; // relative to the current axes
    glm::vec3 _translation; // relative to the previous axes
    std::map<int, float> _constraints; // keyed on constraint index
};

class BallJoint : public Joint
{
public:
    BallJoint() :
        Joint(), _pivotAxes(glm::mat3()), _direction(glm::vec2(0, 0)), _spin(0) {}
    BallJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        Joint(translation, rotation), _pivotAxes(glm::mat3()), _direction(glm::vec2(0,0)), _spin(0) {}
    BallJoint(const glm::vec3& translation, const glm::vec3& w) :
        Joint(translation, w), _pivotAxes(glm::mat3()), _direction(glm::vec2(0, 0)), _spin(0) {}
    BallJoint(const glm::vec3& translation, const glm::mat3& R) :
        Joint(translation, R), _pivotAxes(glm::mat3()), _direction(glm::vec2(0, 0)), _spin(0) {}

    void setPivotAxes(const glm::mat3&);
    void setPivotAxes(const glm::vec3& z, const glm::vec3& y);
    void setSpin(const float&);
    void setDirection(const glm::vec2&);
    void setDirection(const glm::vec3&);

    void setRotation(const AxisAngleRotation2&);
    void setRotation(const glm::vec3&);
    void setRotation(const glm::mat3&);
    void setRotationAxis(const glm::vec3&);
    void setRotationAxis(const glm::vec2& axis);
    void setRotationAngle(const float& angle);

    glm::mat3 rotRelPivR() const;
    glm::vec3 rotRelPiv3() const { return axisAngle3(rotRelPivR()); }
    AxisAngleRotation2 rotRelPiv2() const { return axisAngle2(rotRelPivR()); }

private:
    // For _spin=0 and _direction=(0,0), the segment points along _pivotAxes.z with "up-direction" _pivotAxes.y
    // For _spin=pi/2 and _direction=(pi/2,pi/2), the segment points along _pivotAxes.y with "up-direction" _pivotAxes.z
    // That is... _direction orients the segment (by rotating it away from _pivotAxes.z in the most direct manner)
    //            _spin rotates the segment about its own reoriented main axis
    // We have defined the ball joint parameters in this way since the most natural constraints are in this context
    // e.g. constraining _direction.theta from [0,pi/2) restricts the segment's main-axis to the upper hemisphere (as defined by _pivotAxes.z)
    glm::mat3 _pivotAxes;
    glm::vec2 _direction;
    float _spin; // We have intentionally avoided using AxisAngleRotation2 here, since [_direction,_spin] do not define a rotation in the usual sense
};


#endif