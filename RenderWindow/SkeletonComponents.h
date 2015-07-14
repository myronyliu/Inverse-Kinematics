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
    friend class HalfJoint;
public:
    Bone() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2()) {}
    void draw(const float& scale = 1) const;
    virtual void doDraw(const float& scale = 0.2) const {
        GlutDraw::drawSphere(glm::vec3(0, 0, 0), scale);
    }

    void attach(HalfJoint* halfJoint);
    void dettach(HalfJoint* halfJoint);
    std::set<HalfJoint*> halfJoints() const {
        return _halfJoints;
    }
    std::map<HalfJoint*, Bone*> neighbors() const;
    HalfJoint* getLink(Bone* neighbor) const;

    glm::vec3 translation() const { return _translation; }
    AxisAngleRotation2 rotation2() const { return _rotation; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationR() const { return _rotation.rotationMatrix(); }

protected:
    // The following are relative to some global system
    // I don't think these are really necessary since it can be reclaimed by traversing the joints elsewhere all the way to this Body
    // However, this is convenient as it will serve the function of _globalRotations and _globalTranslations that was once in Scene::Arm
    glm::vec3 _translation;
    AxisAngleRotation2 _rotation;

    std::set<HalfJoint*> _halfJoints;
    // Moving around a "non-cyclic" skeleton of bodies and joints amounts to tree traversal (with a list of visited nodes)
    // Starting from any root Body, access _halfJoints[i].opposingHalfJoint().anchor() to get the bodies adjacent to this one.
    // Once a body has been processed, add it to the list of
private:
};





class HalfJoint
{
    friend class Bone;
public:
    HalfJoint() :
        _translationToTargetBone(glm::vec3(0, 0, 0)), _rotationToTargetBone(AxisAngleRotation2(glm::vec2(0, 0), 0)) {}
    HalfJoint(const glm::vec3& translation, const AxisAngleRotation2& rotation) :
        _translationToTargetBone(translation), _rotationToTargetBone(rotation) {}
    HalfJoint(const glm::vec3& translation, const glm::vec3& w) :
        _translationToTargetBone(translation), _rotationToTargetBone(AxisAngleRotation2(w)) {}
    HalfJoint(const glm::vec3& translation, const glm::mat3& R) :
        _translationToTargetBone(translation), _rotationToTargetBone(AxisAngleRotation2(R)) {}

    // FUNCTIONS THAT MUST BE DEFINED IN CHILDREN CLASSES
    virtual void constrain() {}
    virtual std::map<int, float> getParams() const { return std::map<int, float>(); }
    virtual void setParamsFreely(std::map<int, float>) {}
    virtual void perturbFreely() {}
    virtual float reach() const { return 0; }
    virtual void doDraw(const float&) const {}
    
    void draw(const float& scale) const {
        GlutDraw::drawCylinder(-_translationToTargetBone / 2.0f, -_translationToTargetBone / 2.0f, 0.02);
        glPushMatrix();
        pushTranslation(-_translationToTargetBone);
        pushRotation(-_rotationToTargetBone);
        doDraw(scale);
        glPopMatrix();
    }
    void setConstraint(const int& key, const float& value) { _constraints[key] = value; }
    void perturb() { perturbFreely(); constrain(); }
    void setParams(const std::map<int, float>& params) { setParamsFreely(params); constrain(); }
    void restore() {}
    void backup() {}

    int nParams() const { return getParams().size(); }

    /////////////////
    //// SETTERS ////
    /////////////////

    void setTranslationToTarget(const glm::vec3& translation) { _translationToTargetBone = translation; }

    void setRotationToTarget(const AxisAngleRotation2& rotation) {
        _rotationToTargetBone = rotation;
        _rotationToTargetBone.clamp();
    }
    void setRotationToTarget(const glm::vec3& w) {
        _rotationToTargetBone = AxisAngleRotation2(w);
        _rotationToTargetBone.clamp();
    }
    void setRotationToTarget(const glm::mat3& R) {
        _rotationToTargetBone = AxisAngleRotation2(R);
        _rotationToTargetBone.clamp();
    }
    void setOpposingHalfJoint(HalfJoint* opposingHalfJoint) {
        _opposingHalfJoint = opposingHalfJoint;
        opposingHalfJoint->setOpposingHalfJoint_dummy(this);
    }
    void setTargetBone(Bone*);
    void attach(Bone* target);
    void dettach();

    /////////////////
    //// GETTERS ////
    /////////////////

    glm::vec3 translationToTargetBone() const { return _translationToTargetBone; }
    AxisAngleRotation2 rotationToTargetBone() const { return _rotationToTargetBone; }
    AxisAngleRotation2 rotationToTargetBone2() const { return _rotationToTargetBone; }
    glm::vec3 rotationToTargetBone3() const { return _rotationToTargetBone.axisAngleRotation3(); }
    glm::mat3 rotationToTargetBoneR() const { return _rotationToTargetBone.rotationMatrix(); }
    HalfJoint* opposingHalfJoint() const { return _opposingHalfJoint; }
    Bone* targetBone() const { return _targetBone; }
    Bone* anchorBone() const {
        if (_opposingHalfJoint == NULL) return NULL;
        else return _opposingHalfJoint->targetBone();
    }

    //////////////////////////// These depend on the parameterization of the rotation, so they are actually pure virtual functions in concept
    //// DERIVED QUANTITIES //// The "pivot" is simply "RotationMatrix(_rotation)" i.e. the local coordinate system relative to the body
    ////////////////////////////

    virtual glm::vec3 halfJointTranslation() const { return glm::vec3(); }
    virtual glm::mat3 halfJointRotationR() const { return glm::mat3(); }
    virtual glm::vec3 halfJointRotation3() const { return glm::vec3(); }
    virtual AxisAngleRotation2 halfJointRotation2() const { return AxisAngleRotation2(); }
    virtual AxisAngleRotation2 halfJointRotation() const { return AxisAngleRotation2(); }

    std::pair<glm::vec3, AxisAngleRotation2> alignAnchorToTarget() const {
        glm::mat3 R0 = (-_opposingHalfJoint->rotationToTargetBone()).rotationMatrix();
        glm::mat3 R1 = (-_opposingHalfJoint->halfJointRotation()).rotationMatrix();
        glm::mat3 AnchorToJoint = Math::composeLocalTransforms(R0, R1);
        glm::mat3 R2 = halfJointRotation().rotationMatrix();
        glm::mat3 R3 = rotationToTargetBone().rotationMatrix();
        glm::mat3 JointToTarget = Math::composeLocalTransforms(R2, R3);

        glm::mat3 AnchorToTarget = Math::composeLocalTransforms(AnchorToJoint, JointToTarget);

        glm::vec3 t = -_opposingHalfJoint->translationToTargetBone();
        t += JointToTarget*translationToTargetBone();
        // Express the "translation from this halfjoint to its target" from the target-coordinates (default)...
        // ... to the joint-coordinates.
        // NOTE that this changing coordinates is inverse(targetAxes-->jointAxes), hence "jointToTarget"

        return std::pair<glm::vec3, AxisAngleRotation2>(t, AxisAngleRotation2(AnchorToTarget));
    }
    std::pair<glm::vec3, AxisAngleRotation2> alignTargetToAnchor() const {
        glm::vec3 t;
        AxisAngleRotation2 aa;
        std::tie(t, aa) = alignAnchorToTarget();
        return std::pair<glm::vec3, AxisAngleRotation2>(-t, -aa);
    }


    void attach_dummy(Bone* targetBone) { _targetBone = targetBone; }

protected:
    HalfJoint* _opposingHalfJoint;
    // The other half of the joint that, together with this one, ...
    // ... defines a full joint: with separate degrees of freedom on either side

    Bone* _targetBone;
    // _anchor is the Body that the HalfJoint is anchored to.
    // "Anchored" means that modifying the joint parameters
    // ... (e.g. _mainAxis and _spin for HalfBallJoint)
    // ... has no effect on the orientation and position of _anchor.
    // To get the Body that is "hinged" to this HalfJoint ...
    // ... access _opposingHalfJoint's _anchor

    glm::vec3 _translationToTargetBone;         // These are the translation and orientation relative to the _anchor ...
    AxisAngleRotation2 _rotationToTargetBone;   // ... as expressed in the local coordinate system of the _anchor

    std::map<int, float> _constraints;  // Constraints are keyed on indices of our choosing

private:
    void setOpposingHalfJoint_dummy(HalfJoint* opposingHalfJoint) { _opposingHalfJoint = opposingHalfJoint; }
};

#include "HalfBallJoint.h"

#endif