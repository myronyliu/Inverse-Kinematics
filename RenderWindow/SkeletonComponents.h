#ifndef _SKELETONCOMPONENTS_H_
#define _SKELETONCOMPONENTS_H_

#include "stdafx.h"
#include "Rotation.h"
#include "GlutDraw.h"
#include "utils.h"
#include "Math.h"

static GLfloat red[] = { 1, 0, 0, 1 };
static GLfloat green[] = { 0, 1, 0, 1 };
static GLfloat blue[] = { 0, 0, 1, 1 };
static GLfloat white[] = { 1, 1, 1, 1 };

class Joint;

class Bone
{
    friend class Joint;
public:
    Bone(): _anchoredJoints(std::set<Joint*>()) {}
    Bone(std::vector<Joint*> joints) { for (auto joint : joints) attach(joint); }
    void draw(const float& scale = 1) const;
    virtual void doDraw(const float& scale = 0.2) const;

    Joint* attach(Joint*);
    void unattach(Joint*);

    Joint* couple(Joint*);
    void uncouple(Joint*);

    std::set<Joint*> anchoredJoints() const { return _anchoredJoints; }
    std::set<Joint*> targetingJoints() const { return _targetingJoints; }

    // The terminology is as follows:
    // ... The shoulder bone COUPLES to the arm bone
    // ... The arm bone is ATTACHING to the shoulder bone
    // Therefore,   calling ShoulderBone.coupledBones() will return { shoulder->arm joint : arm      bone }
    // In contrast, calling    ArmBone.attachingBones() will return { shoulder->arm joint : shoulder bone }

    std::map<Joint*, Bone*> coupledBones() const;
    std::map<Joint*, Bone*> attachingBones() const;

    Joint* getLink(Bone* neighbor) const;

protected:
    std::set<Joint*> _anchoredJoints; // these are the joints that are ANCHORED to this body
    std::set<Joint*> _targetingJoints;
private:
};





class Joint
{
    friend class Bone;
public:
    Joint();
    Joint(const int&, const float& = 1);
    Joint(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&);

    /////////////////
    //// DRAWING ////
    /////////////////

    void draw(const float& scale) const;
    virtual void drawJoint(const float&) const {}
    virtual void drawPivot(const float&) const {}

    //////////////////////////
    //// PARAMETERIZATION ////
    //////////////////////////

    virtual void constrainParams() {}
    virtual std::map<int, float> adjustableParams() const { return _params; }
    void setParams(const std::map<int, float>& params_unconstrained);
    void setParam(const int& key, const float& value);
    void setConstraint(const int& key, const float& value);

    void restore() { _params = _stashedParams; }
    void backup() { _stashedParams = _params; }

    void perturb(const float& scale = 1) { perturbParams(scale); constrainParams(); buildTransformsFromParams(); }
    virtual float reach() const { return 0; }

    /////////////////
    //// SETTERS ////
    /////////////////

    void setTranslationFromAnchor(const glm::vec3& translation) { _translationFromAnchor = translation; }
    void setRotationFromAnchor(const AxisAngleRotation2& rotation) { _rotationFromAnchor = rotation; _rotationFromAnchor.clamp(); }
    void setRotationFromAnchor(const glm::vec3& w) { _rotationFromAnchor = AxisAngleRotation2(w); _rotationFromAnchor.clamp(); }
    void setRotationFromAnchor(const glm::mat3& R) { _rotationFromAnchor = AxisAngleRotation2(R); _rotationFromAnchor.clamp(); }
    Bone* couple(Bone* target) { _target = target; return target; }
    void decouple() { _target = NULL; }

    void setTranslationToTarget(const glm::vec3& translation) { _translationFromAnchor = translation; }
    void setRotationToTarget(const AxisAngleRotation2& rotation) { _rotationFromAnchor = rotation; _rotationFromAnchor.clamp(); }
    void setRotationToTarget(const glm::vec3& w) { _rotationFromAnchor = AxisAngleRotation2(w); _rotationFromAnchor.clamp(); }
    void setRotationToTarget(const glm::mat3& R) { _rotationFromAnchor = AxisAngleRotation2(R); _rotationFromAnchor.clamp(); }
    void attach(Bone* target);
    void unattach();

    void setJointTranslation(const glm::vec3& translation);
    void setJointRotation(const AxisAngleRotation2& rotation);
    void setJointRotation(const glm::vec3& w);
    void setJointRotation(const glm::mat3& R);

    /////////////////
    //// GETTERS ////
    /////////////////

    std::map<int, float> params() const { return _params; }
    bool getConstraint(const int& key, float& value) const;
    bool getParam(const int& key, float& value) const;

    Bone* anchor() const { return _anchor; }
    glm::vec3 translationFromAnchor() const { return _translationFromAnchor; }
    AxisAngleRotation2 rotationFromAnchor() const { return _rotationFromAnchor; }
    AxisAngleRotation2 rotationFromAnchor2() const { return _rotationFromAnchor; }
    glm::vec3 rotationFromAnchor3() const { return _rotationFromAnchor.axisAngleRotation3(); }
    glm::mat3 rotationFromAnchorR() const { return _rotationFromAnchor.rotationMatrix(); }

    Bone* target() const { return _target; }
    glm::vec3 translationToTarget() const { return _translationToTarget; }
    AxisAngleRotation2 rotationToTarget() const { return _rotationToTarget; }
    AxisAngleRotation2 rotationToTarget2() const { return _rotationToTarget; }
    glm::vec3 rotationToTarget3() const { return _rotationToTarget.axisAngleRotation3(); }
    glm::mat3 rotationToTargetR() const { return _rotationToTarget.rotationMatrix(); }

    glm::vec3 jointTranslation() const { return _jointTranslation; }
    AxisAngleRotation2 jointRotation() const { return _jointRotation; }
    AxisAngleRotation2 jointRotation2() const { return _jointRotation; }
    glm::vec3 jointRotation3() const { return _jointRotation.axisAngleRotation3(); }
    glm::mat3 jointRotationR() const { return _jointRotation.rotationMatrix(); }

    ////////////////////////////
    //// DERIVED QUANTITIES ////
    ////////////////////////////

    std::pair<glm::vec3, AxisAngleRotation2> alignAnchorToTarget() const;

protected:
    //////////////////////////////////////////////////////////////////
    //// DANGEROUS FUNCTIONS THAT SHOULD NOT BE ACCESSED PUBLICLY ////
    //////////////////////////////////////////////////////////////////

    virtual void perturbParams(const float& scale) {}
    virtual void buildTransformsFromParams() {}
    virtual void buildParamsFromTransforms() {}

    //////////////////////////
    //// MEMBER VARIABLES ////
    //////////////////////////

    Bone* _anchor;
    Bone* _target;

    glm::vec3 _translationFromAnchor;       // IN THE COORDINATE FRAME OF THE ANCHOR
    AxisAngleRotation2 _rotationFromAnchor;

    glm::vec3 _jointTranslation;            // IN THE COORDINATE FRAME OF THE JOINT's "BASE" (in "default" configuration)
    AxisAngleRotation2 _jointRotation;

    glm::vec3 _translationToTarget;         // IN THE COORDINATE FRAME OF THE JOINT's "TIP"
    AxisAngleRotation2 _rotationToTarget;



    std::map<int, float> _constraints;  // Constraints are keyed on indices of our choosing
    std::map<int, float> _params;

    std::map<int, float> _stashedParams;
};

#include "BallJoint.h"

#endif