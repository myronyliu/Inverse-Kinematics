#ifndef _HALFBALLJOINT_H_
#define _HALFBALLJOINT_H_

#include "stdafx.h"
#include "SkeletonComponents.h"
#include "Rotation.h"
#include "Math.h"

// PARAMETERS FOR BALLJOINT:
// 0 : main-axis theta
// 1 : main-axis phi
// 2 : rotation angle about the main-axis

class BallJoint : public Joint
{
public:
    BallJoint() :
        Joint() {}
    BallJoint(const glm::vec3& translationFromAnchor, const glm::vec3& rotationFromAnchor, const glm::vec3& translationToTarget, const glm::vec3& rotationToTarget) :
        Joint(translationFromAnchor, rotationFromAnchor, translationToTarget, rotationToTarget) {}

    std::map<int, float> adjustableParams() const;
    void buildTransformsFromParams() { _jointRotation = AxisAngleRotation2(AxisSpinRotation(glm::vec2(_params[0], _params[1]), _params[2])); }
    void buildParamsFromTransforms();
    void constrainParams();
    void perturbParams(const float& scale);

    void drawJoint(const float& radius) const;
    void drawPivot(const float& radius) const;
private:
};


#endif