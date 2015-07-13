#ifndef _HALFJOINT_H_
#define _HALFJOINT_H_

#include "stdafx.h"
#include "utils.h"
#include "math.h"
#include "GlutDraw.h"
#include "Body.h"

class Body;

// TODO: add Track-Joint class (similar to PRISM except with translation along a specified path)
// NOTE: try to limit all "clamping of _rotation" to root Joint class

class HalfJoint
{
public:
    HalfJoint() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2(glm::vec2(0, 0), 0)) {}
    HalfJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        _translation(translation), _rotation(rotation) {}
    HalfJoint(const glm::vec3& translation, const glm::vec3& w) :
        _translation(translation), _rotation(AxisAngleRotation2(w)) {}
    HalfJoint(const glm::vec3& translation, const glm::mat3& R) :
        _translation(translation), _rotation(AxisAngleRotation2(R)) {}

    // FUNCTIONS THAT MUST BE DEFINED IN CHILDREN CLASSES
    virtual void constrain() {}
    virtual std::map<int, float> getParams() const { return std::map<int, float>(); }
    virtual void setParamsFreely(std::map<int, float>) {}
    virtual void perturbFreely() {}
    virtual void draw(const float&) const {}
    virtual float reach() const { return 0; }

    void setConstraint(const int& key, const float& value) { _constraints[key] = value; }
    void perturb() { perturbFreely(); clamp(); }
    void setParams(const std::map<int, float>& params) { setParamsFreely(params); clamp(); }
    void restore() {}
    void backup() {}

    int nParams() const { return getParams().size(); }

    /////////////////
    //// SETTERS ////
    /////////////////

    void setTranslation(const glm::vec3& translation) { _translation = translation; }
    void setRotation(const AxisAngleRotation2& rotation) { _rotation = rotation; _rotation.clamp(); }
    void setRotation(const glm::vec3& w) { _rotation = AxisAngleRotation2(w); _rotation.clamp(); }
    void setRotation(const glm::mat3& R) { _rotation = AxisAngleRotation2(R); _rotation.clamp(); }
    void setRotationAxis(const glm::vec3& w) { _rotation._axis = glm::vec2(acos(w[2] / glm::length(w)), atan2(w[1], w[0])); _rotation.clamp(); }
    void setRotationAxis(const glm::vec2& axis) { _rotation._axis = axis; _rotation.clamp(); }
    void setRotationTheta(const float& theta) { _rotation._axis[0] = theta; _rotation.clamp(); }
    void setRotationPhi(const float& phi) { _rotation._axis[1] = phi; _rotation.clamp(); }
    void setRotationAngle(const float& angle) { _rotation._angle = angle; _rotation.clamp(); }

    /////////////////
    //// GETTERS ////
    /////////////////

    glm::vec3 translation() const { return _translation; }
    AxisAngleRotation2 rotation2() const { return _rotation; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationR() const { return _rotation.rotationMatrix(); }

    //////////////////////////// These depend on the parameterization of the rotation, so they are actually pure virtual functions in concept
    //// DERIVED QUANTITIES //// The "pivot" is simply "RotationMatrix(_rotation)" i.e. the local coordinate system relative to the body
    ////////////////////////////

    virtual glm::vec3 translationFromPivot() const { return glm::vec3(); }
    virtual glm::mat3 rotationFromPivotR() const { return glm::mat3(); }
    virtual glm::vec3 rotationFromPivot3() const { return glm::vec3(); }
    virtual AxisAngleRotation2 rotationFromPivot3() const { return AxisAngleRotation2(); }


protected:
    HalfJoint* _partner;    // The other half of the joint that, together with this one, ...
                            // ... defines a full joint: with separate degrees of freedom on either side

    Body* _anchor;  // _anchor is the Body that the HalfJoint is anchored to.
                    // "Anchored" means that modifying the joint parameters
                    // ... (e.g. _mainAxis and _spin for HalfBallJoint)
                    // ... has no effect on the orientation and position of _anchor.
                    // To get the Body that is "hinged" to this HalfJoint ...
                    // ... access _partner's _anchor

    glm::vec3 _translation;         // These are the translation and orientation relative to the _anchor ...
    AxisAngleRotation2 _rotation;   // ... as expressed in the local coordinate system of the _anchor

    std::map<int, float> _constraints;  // Constraints are keyed on indices of our choosing
};

#include "HalfBallJoint.h"
#include "HalfPrismJoint.h"

#endif