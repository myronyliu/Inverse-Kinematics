#ifndef _MATH_H_
#define _MATH_H_

#include "stdafx.h"
#include "Rotation.h"

namespace Math {
    glm::vec3 rotate(const glm::vec3& v, const glm::vec3& w);
    float clamp(const float& lowerBound, const float& value, const float& upperBound);

    glm::mat3 rotationMatrix(const glm::vec3&);
    glm::mat3 rotationMatrix(const AxisAngleRotation2&);
    glm::mat3 rotationMatrix(const glm::vec2&, const float&);
    glm::mat3 rotationMatrix(const float&, const glm::vec2&);
    // The following gives the matrix for changing from the former coordinate axes to the latter coordinate axes
    // e.g. To reexpress v (currently expressed in the former basis) in the latter basis,
    // ...  use basisChangeMatrix(former basis, latter basis)*v
    glm::mat3 basisChangeMatrix(const glm::mat3&, const glm::mat3&);
    glm::mat3 composeLocalTransforms(const glm::mat3&, const glm::mat3&);
    glm::vec3 composeLocalRotations(const glm::vec3&, const glm::vec3&);
    AxisAngleRotation2 composeLocalRotations(const AxisAngleRotation2&, const AxisAngleRotation2&);

    glm::vec3 axisAngleRotation3(const glm::vec2& axis, const float& angle);
    glm::vec3 axisAngleRotation3(const float& angle, const glm::vec2& axis);
    glm::vec3 axisAngleRotation3(const AxisAngleRotation2& axisAngle);
    glm::vec3 axisAngleRotation3(const glm::mat3& R);

    AxisAngleRotation2 axisAngleRotation2(const glm::vec3&);
    AxisAngleRotation2 axisAngleRotation2(const glm::mat3&);

    glm::mat3 matrixAlignVECtoZ(const glm::vec3&);
    glm::mat3 matrixAlignZtoVEC(const glm::vec3&);
    glm::vec3 axisAngleAlignVECtoZ3(const glm::vec3&);
    glm::vec3 axisAngleAlignZtoVEC3(const glm::vec3&);
    AxisAngleRotation2 axisAngleAlignVECtoZ2(const glm::vec3&);
    AxisAngleRotation2 axisAngleAlignZtoVEC2(const glm::vec3&);
    glm::vec3 axisAngleAlignFirstToSecond(const glm::vec3&, const glm::vec3&);

    glm::vec3 axisAngleAlignVECStoZY3(const glm::vec3& zIn, const glm::vec3& yIn);
    AxisAngleRotation2 axisAngleAlignVECStoZY2(const glm::vec3& zIn, const glm::vec3& yIn);

    glm::vec3 axisAngleAlignZYtoVECS3(const glm::vec3& zIn, const glm::vec3& yIn);
    AxisAngleRotation2 axisAngleAlignZYtoVECS2(const glm::vec3& zIn, const glm::vec3& yIn);

}

#endif