#ifndef _JOINT_H_
#define _JOINT_H_

#include "stdafx.h"
#include "utils.h"
#include "math.h"
#include "GlutDraw.h"
#include "Body.h"

class Body;

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
    virtual std::vector<float> getParams() const { return std::vector<float>(0); };
    virtual void setParamsFreely(const std::vector<float>&) {};
    virtual void perturb() {};
    virtual void draw(const float&) const {};
    virtual float reach() const { return 0; }
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


    std::pair<glm::vec3, AxisAngleRotation2> tryParams(const std::vector<float>&) ;

    int nParams() const { return getParams().size(); }
    
    // The following are virtual, because sometimes they affect the inherited class's member variables as well
    // ... in which case, we need to redefine these functions
    virtual void setTranslation(const glm::vec3& translation) { _translation = translation; }
    virtual void setRotation(const AxisAngleRotation2& rotation) { _rotation = rotation; _rotation.clamp(); }
    virtual void setRotation(const glm::vec3& w) { _rotation = AxisAngleRotation2(w); _rotation.clamp(); }
    virtual void setRotation(const glm::mat3& R) { _rotation = AxisAngleRotation2(R); _rotation.clamp(); }
    virtual void setRotationAxis(const glm::vec3& w) {
        _rotation._axis = glm::vec2(acos(w[2] / glm::length(w)), atan2(w[1], w[0]));
        _rotation.clamp();
    }
    virtual void setRotationAxis(const glm::vec2& axis) { _rotation._axis = axis; _rotation.clamp(); }
    virtual void setRotationTheta(const float& theta) { _rotation._axis[0] = theta; _rotation.clamp(); }
    virtual void setRotationPhi(const float& phi) { _rotation._axis[1] = phi; _rotation.clamp(); }
    virtual void setRotationAngle(const float& angle) { _rotation._angle = angle; _rotation.clamp(); }

    glm::vec3 translation() const { return _translation; }
    AxisAngleRotation2 rotation2() const { return _rotation; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationR() const { return _rotation.rotationMatrix(); }

protected:
    Body* _left;
    Body* _right;

    // Relative to the LEFT Body
    glm::vec3 _translation;
    AxisAngleRotation2 _rotation;

    // Relative to the RIGHT Body
    glm::vec3 _leftTranslation;
    AxisAngleRotation2 _rightTranslation;

    // STASHES for backup and restore
    glm::vec3 _translation_stash;
    AxisAngleRotation2 _rotation_stash;
};

#include "BallJoint.h"
#include "PrismJoint.h"

#endif