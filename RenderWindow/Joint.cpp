#include "SkeletonComponents.h"
#include "GlutDraw.h"

using namespace std;
using namespace glm;
using namespace Math;


//////////////////////
//// CONSTRUCTORS ////
//////////////////////

Joint::Joint() :
_translationFromAnchor(glm::vec3(0, 0, 0)),
_rotationFromAnchor(AxisAngleRotation2(glm::vec2(0, 0), 0)),
_jointTranslation(glm::vec3(0, 0, 0)),
_jointRotation(AxisAngleRotation2(glm::vec2(0, 0), 0)),
_translationToTarget(glm::vec3(0, 0, 0)),
_rotationToTarget(AxisAngleRotation2(glm::vec2(0, 0), 0))
{}

Joint::Joint(const glm::vec3& translationFromAnchor, const glm::vec3& wFromAnchor, const glm::vec3& translationToTarget, const glm::vec3& wToTarget) :
_translationFromAnchor(translationFromAnchor),
_rotationFromAnchor(AxisAngleRotation2(wFromAnchor)),
_jointTranslation(glm::vec3(0, 0, 0)),
_jointRotation(AxisAngleRotation2(glm::vec2(0, 0), 0)),
_translationToTarget(translationToTarget),
_rotationToTarget(AxisAngleRotation2(wToTarget))
{}

/////////////////
//// SETTERS ////
/////////////////

void Joint::setJointTranslation(const glm::vec3& translation) {
    _jointTranslation = translation;
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}
void Joint::setJointRotation(const AxisAngleRotation2& rotation) {
    _jointRotation = rotation;
    _jointRotation.clamp();
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}
void Joint::setJointRotation(const glm::vec3& w) {
    _jointRotation = AxisAngleRotation2(w);
    _jointRotation.clamp();
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}
void Joint::setJointRotation(const glm::mat3& R) {
    _jointRotation = AxisAngleRotation2(R);
    _jointRotation.clamp();
    buildParamsFromTransforms();
    constrainParams();
    buildTransformsFromParams();
}

void Joint::setConstraint(const int& key, const float& value) {
    _constraints[key] = value;
    constrainParams();
    buildTransformsFromParams();
}



std::pair<glm::vec3, AxisAngleRotation2> Joint::alignAnchorToTarget() const {

    glm::mat3 JointFrameAxes_AnchorFrame = _rotationFromAnchor.rotationMatrix();

    // Express the joint rotation in the Anchor frame and use it to compute the total rotation from anchor to joint
    glm::mat3 JointR_AnchorFrame = revertFromBasis(_jointRotation.rotationMatrix(), JointFrameAxes_AnchorFrame);
    glm::mat3 AnchorToJointR_AnchorFrame = JointR_AnchorFrame*JointFrameAxes_AnchorFrame;

    // Express the joint translation in the Anchor frame. Then sum it with the translation from the Anchor (which is already expressed in the Anchor frame)
    glm::vec3 t = _translationFromAnchor + AnchorToJointR_AnchorFrame*(_jointTranslation+_translationToTarget);

    // Express the rotation vector between the Target and the Joint in the frame of the Anchor
    glm::vec3 TargetW_AnchorFrame = AnchorToJointR_AnchorFrame*_rotationToTarget.axisAngleRotation3();

    // Compute the total rotation from the Anchor Frame to the Target frame
    glm::mat3 R = rotationMatrix(TargetW_AnchorFrame)*AnchorToJointR_AnchorFrame;

    return std::pair<glm::vec3, AxisAngleRotation2>(t, AxisAngleRotation2(R));
}

void Joint::draw(const float& scale) const {
    GlutDraw::drawCylinder(_translationFromAnchor / 2.0f, _translationFromAnchor / 2.0f, 0.02);

    glPushMatrix();
    pushTranslation(_translationFromAnchor);
    pushRotation(_rotationFromAnchor);

    //drawPivot(scale);

    glPushMatrix();
    pushTranslation(_jointTranslation);
    pushRotation(_jointRotation);

    drawJoint(scale);
    GlutDraw::drawCylinder(_translationToTarget / 2.0f, _translationToTarget / 2.0f, 0.02);

    glPopMatrix();
    glPopMatrix();
}