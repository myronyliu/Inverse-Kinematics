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




std::pair<glm::vec3, AxisAngleRotation2> Joint::alignAnchorToTarget() const {

    glm::mat3 R = composeLocalTransforms(
        _rotationFromAnchor.rotationMatrix(),
        _jointRotation.rotationMatrix());

    glm::vec3 t = _translationFromAnchor + R*(_jointTranslation+_translationToTarget);

    R = composeLocalTransforms(R, _rotationToTarget.rotationMatrix());

    return std::pair<glm::vec3, AxisAngleRotation2>(t, AxisAngleRotation2(R));
}

void Joint::draw(const float& scale) const {
    GlutDraw::drawCylinder(_translationFromAnchor / 2.0f, _translationFromAnchor / 2.0f, 0.02);

    bool transformed0 = false;
    bool transformed1 = false;
    if (_translationFromAnchor != glm::vec3(0, 0, 0) || _rotationFromAnchor.axisAngleRotation3() != glm::vec3(0, 0, 0)) transformed0 = true;
    if (_jointTranslation != glm::vec3(0, 0, 0) || _jointRotation.axisAngleRotation3() != glm::vec3(0, 0, 0)) transformed1 = true;

    if (transformed0) {
        glPushMatrix();
        pushTranslation(_translationFromAnchor);
        pushRotation(_rotationFromAnchor);
    }

    drawPivot(scale);

    if (transformed1) {
        glPushMatrix();
        pushTranslation(_jointTranslation);
        pushRotation(_jointRotation);
    }

    drawJoint(scale);
    GlutDraw::drawCylinder(_translationToTarget / 2.0f, _translationToTarget / 2.0f, 0.02);

    if (transformed1)
        glPopMatrix();
    if (transformed0)
        glPopMatrix();
}