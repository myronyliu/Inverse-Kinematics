#ifndef _SKELETONCOMPONENTS_H_
#define _SKELETONCOMPONENTS_H_

#include "stdafx.h"
#include "Rotation.h"
#include "GlutDraw.h"
#include "utils.h"
#include "Math.h"

class HalfJoint;

class Bone
{
public:
    Bone() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2()) {}
    void draw(const float& scale = 1) const {
        glPushMatrix();
        pushTranslation(_translation);
        pushRotation(_rotation);
        doDraw(scale);
        glPopMatrix();
    }
    virtual void doDraw(const float& scale = 1) const {
        GlutDraw::drawSphere(glm::vec3(0, 0, 0), scale);
    }
    void attach(HalfJoint* halfJoint) {
        _halfJoints.insert(halfJoint);
    }
    void dettach(HalfJoint* halfJoint) {
        _halfJoints.erase(halfJoint);
    }
    std::set<HalfJoint*> halfJoints() const {
        return _halfJoints;
    }
    std::map<HalfJoint*, Bone*> neighbors() const;
    HalfJoint* getLink(Bone* neighbor) const;

    glm::vec3 translation() const { return _translation; }
    AxisAngleRotation2 rotation2() const { return _rotation; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationR() const { return _rotation.rotationMatrix(); }

private:
    // The following are relative to some global system
    // I don't think these are really necessary since it can be reclaimed by traversing the joints elsewhere all the way to this Body
    // However, this is convenient as it will serve the function of _globalRotations and _globalTranslations that was once in Scene::Arm
    glm::vec3 _translation;
    AxisAngleRotation2 _rotation;

    std::set<HalfJoint*> _halfJoints;
    // Moving around a "non-cyclic" skeleton of bodies and joints amounts to tree traversal (with a list of visited nodes)
    // Starting from any root Body, access _halfJoints[i].partner().anchor() to get the bodies adjacent to this one.
    // Once a body has been processed, add it to the list of 
};





class HalfJoint
{
public:
    HalfJoint() :
        _translationToTarget(glm::vec3(0, 0, 0)), _rotationToTarget(AxisAngleRotation2(glm::vec2(0, 0), 0)) {}
    HalfJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        _translationToTarget(translation), _rotationToTarget(rotation) {}
    HalfJoint(const glm::vec3& translation, const glm::vec3& w) :
        _translationToTarget(translation), _rotationToTarget(AxisAngleRotation2(w)) {}
    HalfJoint(const glm::vec3& translation, const glm::mat3& R) :
        _translationToTarget(translation), _rotationToTarget(AxisAngleRotation2(R)) {}

    // FUNCTIONS THAT MUST BE DEFINED IN CHILDREN CLASSES
    virtual void constrain() {}
    virtual std::map<int, float> getParams() const { return std::map<int, float>(); }
    virtual void setParamsFreely(std::map<int, float>) {}
    virtual void perturbFreely() {}
    virtual void draw(const float&) const {}
    virtual float reach() const { return 0; }

    void setConstraint(const int& key, const float& value) { _constraints[key] = value; }
    void perturb() { perturbFreely(); constrain(); }
    void setParams(const std::map<int, float>& params) { setParamsFreely(params); constrain(); }
    void restore() {}
    void backup() {}

    int nParams() const { return getParams().size(); }

    /////////////////
    //// SETTERS ////
    /////////////////

    void setTranslationToTarget(const glm::vec3& translation) { _translationToTarget = translation; }

    void setRotationToTarget(const AxisAngleRotation2& rotation) {
        _rotationToTarget = rotation;
        _rotationToTarget.clamp();
    }
    void setRotationToTarget(const glm::vec3& w) {
        _rotationToTarget = AxisAngleRotation2(w);
        _rotationToTarget.clamp();
    }
    void setRotationToTarget(const glm::mat3& R) {
        _rotationToTarget = AxisAngleRotation2(R);
        _rotationToTarget.clamp();
    }

    /////////////////
    //// GETTERS ////
    /////////////////

    glm::vec3 translationToTarget() const { return _translationToTarget; }
    AxisAngleRotation2 rotationToTarget() const { return _rotationToTarget; }
    AxisAngleRotation2 rotationToTarget2() const { return _rotationToTarget; }
    glm::vec3 rotationToTarget3() const { return _rotationToTarget.axisAngleRotation3(); }
    glm::mat3 rotationToTargetR() const { return _rotationToTarget.rotationMatrix(); }
    HalfJoint* partner() const { return _partner; }
    Bone* target() const { return _target; }
    Bone* anchor() const { return _partner->target(); }

    //////////////////////////// These depend on the parameterization of the rotation, so they are actually pure virtual functions in concept
    //// DERIVED QUANTITIES //// The "pivot" is simply "RotationMatrix(_rotation)" i.e. the local coordinate system relative to the body
    ////////////////////////////

    virtual glm::vec3 halfJointTranslation() const { return glm::vec3(); }
    virtual glm::mat3 halfJointRotationR() const { return glm::mat3(); }
    virtual glm::vec3 halfJointRotation3() const { return glm::vec3(); }
    virtual AxisAngleRotation2 halfJointRotation2() const { return AxisAngleRotation2(); }
    virtual AxisAngleRotation2 halfJointRotation() const { return AxisAngleRotation2(); }

    std::pair<glm::vec3, AxisAngleRotation2> alignAnchorToTarget() const {
        glm::vec3 t = -_partner->translationToTarget();
        glm::mat3 R0 = (-_partner->rotationToTarget()).rotationMatrix();
        glm::mat3 R1 = (-_partner->halfJointRotation()).rotationMatrix();
        glm::mat3 R = Math::composeLocalTransforms(R0, R1);
        t += R*translationToTarget();
        R = Math::composeLocalTransforms(R, halfJointRotation().rotationMatrix());
        R = Math::composeLocalTransforms(R, rotationToTarget().rotationMatrix());
        return std::pair<glm::vec3, AxisAngleRotation2>(t, AxisAngleRotation2(R));
    }
    std::pair<glm::vec3, AxisAngleRotation2> alignTargetToAnchor() const {
        glm::vec3 t;
        AxisAngleRotation2 aa;
        std::tie(t, aa) = alignAnchorToTarget();
        return std::pair<glm::vec3, AxisAngleRotation2>(-t, -aa);
    }

protected:
    HalfJoint* _partner;    // The other half of the joint that, together with this one, ...
    // ... defines a full joint: with separate degrees of freedom on either side

    Bone* _target;  // _anchor is the Body that the HalfJoint is anchored to.
    // "Anchored" means that modifying the joint parameters
    // ... (e.g. _mainAxis and _spin for HalfBallJoint)
    // ... has no effect on the orientation and position of _anchor.
    // To get the Body that is "hinged" to this HalfJoint ...
    // ... access _partner's _anchor

    glm::vec3 _translationToTarget;         // These are the translation and orientation relative to the _anchor ...
    AxisAngleRotation2 _rotationToTarget;   // ... as expressed in the local coordinate system of the _anchor

    std::map<int, float> _constraints;  // Constraints are keyed on indices of our choosing
};

#endif