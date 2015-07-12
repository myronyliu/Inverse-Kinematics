
#include "Math.h"
#include "utils.h"


template <class T>
T Math::clamp(const T& lowerBound, const T& value, const T& upperBound) {
    if (value < lowerBound) return lowerBound;
    else if (value > upperBound) return upperBound;
}

glm::mat3 Math::basisChangeMatrix(const glm::mat3& oldBasis, const glm::mat3& newBasis) {
    return glm::inverse(newBasis)*oldBasis;
}

glm::mat3 Math::composeLocalTransforms(const glm::mat3& A, const glm::mat3& B) {
    // A defines a basis, so to convert B's columns (currently in standard basis [xHat,yHat,zHat]) to A's basis
    // we left multiply B by the "basis-change-matrix from standard to A" : A_inv
    // The result is A_inv.B, which after multiplying by the previous transformation A, gives...
    return (glm::inverse(A)*B)*A;
}
glm::vec3 Math::composeLocalRotations(const glm::vec3& w0, const glm::vec3& w1) {
    float angle0 = glm::length(w0);
    float angle1 = glm::length(w1);
    if (angle0 == 0) return w1;
    if (angle1 == 0) return w0;
    glm::mat3 R0 = rotationMatrix(w0);
    glm::vec3 w1_frame0 = R0*w1;
    glm::mat3 R = rotationMatrix(w1_frame0)*R0;
    return axisAngleRotation3(R);
}
AxisAngleRotation2 Math::composeLocalRotations(const AxisAngleRotation2& axisAngle0, const AxisAngleRotation2& axisAngle1) {
    glm::vec3 w = composeLocalRotations(axisAngle0.axisAngleRotation3(), axisAngle1.axisAngleRotation3());
    return AxisAngleRotation2(w);
}

glm::mat3 Math::rotationMatrix(const glm::vec3& w) {

    float theta = glm::length(w);
    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta == 0) return glm::mat3();
    glm::vec3 wHat = w / theta;
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    //if (fabs(theta - M_PI) < 0.000001) {
    if (theta == M_PI) {
        return glm::mat3() + 2.0f * wCross*wCross;
    }
    else {
        return glm::mat3() + sin(theta)*wCross + (1 - cos(theta))*wCross*wCross;
    }
}
glm::mat3 Math::rotationMatrix(const AxisAngleRotation2& axisAngle) {
    return rotationMatrix(axisAngle.axisAngleRotation3());
}
glm::mat3 Math::rotationMatrix(const glm::vec2& axis, const float& angle) {
    return rotationMatrix(axisAngleRotation3(axis, angle));
}
glm::mat3 Math::rotationMatrix(const float& angle, const glm::vec2& axis) {
    return rotationMatrix(axisAngleRotation3(axis, angle));
}


glm::vec3 Math::axisAngleRotation3(const glm::mat3& R) {
    /*float acosArg = (R[0][0] + R[1][1] + R[2][2] - 1) / 2;
    if (fabs(acosArg) >= 0.999999) {
        float theta = (acosArg > 0) ? M_PI : -M_PI;
        if (R[0][0] >= R[1][1] && R[0][0] >= R[2][2]) {
            float x = sqrt((R[0][0] + 1) / 2);
            return theta*glm::vec3(x, R[1][0] / (2 * x), R[2][0] / (2 * x));
        }
        else if (R[1][1] >= R[2][2] && R[1][1] >= R[0][0]) {
            float y = sqrt((R[1][1] + 1) / 2);
            return theta*glm::vec3(R[0][1] / (2 * y), y, R[2][1] / (2 * y));
        }
        else {
            float z = sqrt((R[2][2] + 1) / 2);
            return theta*glm::vec3(R[0][2] / (2 * z), R[1][2] / (2 * z), z);
        }
    }
    else {
        float theta = acos(acosArg);
        if (theta == 0) {
            return glm::vec3(0, 0, 0);
        }
        else {
            return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
        }
    }*/
    float theta = acos(clamp(-1.0f, (R[0][0] + R[1][1] + R[2][2] - 1) / 2, 1.0f));
    if (theta == 0) {
        return glm::vec3(0, 0, 0);
    }
    else if (theta == M_PI) {
        if (R[0][0] >= R[1][1] && R[0][0] >= R[2][2]) {
            float x = sqrt((R[0][0] + 1) / 2);
            return theta*glm::vec3(x, R[1][0] / (2 * x), R[2][0] / (2 * x));
        }
        else if (R[1][1] >= R[2][2] && R[1][1] >= R[0][0]) {
            float y = sqrt((R[1][1] + 1) / 2);
            return theta*glm::vec3(R[0][1] / (2 * y), y, R[2][1] / (2 * y));
        }
        else {
            float z = sqrt((R[2][2] + 1) / 2);
            return theta*glm::vec3(R[0][2] / (2 * z), R[1][2] / (2 * z), z);
        }
    }
    else return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
}
AxisAngleRotation2 Math::axisAngleRotation2(const glm::mat3& R) {
    return axisAngleRotation2(axisAngleRotation3(R));
}

AxisAngleRotation2 Math::axisAngleRotation2(const glm::vec3& w) {
    float angle = glm::length(w);
    if (angle == 0) return AxisAngleRotation2(glm::vec2(0, 0), 0);
    glm::vec2 axis(acos(w[2] / angle), atan2(w[1], w[0]));
    angle -= (2 * M_PI)*floor(angle / (2 * M_PI));
    return AxisAngleRotation2(axis, angle);
}


glm::vec3 Math::axisAngleRotation3(const glm::vec2& axis, const float& angle) {
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}
glm::vec3 Math::axisAngleRotation3(const float& angle, const glm::vec2& axis) {
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}
glm::vec3 Math::axisAngleRotation3(const AxisAngleRotation2& axisAngle) {
    glm::vec2 axis = axisAngle._axis;
    float angle = axisAngle._angle;
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}

glm::vec3 Math::axisAngleAlignVECtoZ3(const glm::vec3& axisIn) {
    glm::vec3 axis = glm::normalize(axisIn);
    if (axis[0] == 0 && axis[1] == 0) {
        if (axis[2] >= 0) return glm::vec3(0, 0, 0);
        else return glm::vec3(M_PI, 0, 0);
    }
    else {
        return (M_PI / 2.0f)*glm::cross(axis, glm::vec3(0, 0, 1));
    }
}

glm::vec3 Math::axisAngleAlignZtoVEC3(const glm::vec3& axis) { return -axisAngleAlignVECtoZ3(axis); }
AxisAngleRotation2 Math::axisAngleAlignVECtoZ2(const glm::vec3& axis) { return axisAngleRotation2(axisAngleAlignVECtoZ3(axis)); }
AxisAngleRotation2 Math::axisAngleAlignZtoVEC2(const glm::vec3& axis) { return axisAngleRotation2(axisAngleAlignZtoVEC3(axis)); }

glm::vec3 Math::axisAngleAlignFirstToSecond(const glm::vec3& axisIn, const glm::vec3& targetIn) {
    glm::vec3 axis = glm::normalize(axisIn);
    glm::vec3 target = glm::normalize(targetIn);
    if (axis[0] == target[0] && axis[1] == target[1] && axis[2] == target[2]) {
        return glm::vec3(0, 0, 0);
    }
    else if (axis[0] == -target[0] && axis[1] == -target[1] && axis[2] == -target[2]) {
        if (fabs(axis[0]) <= fabs(axis[1]) && fabs(axis[0]) <= fabs(axis[2])) {
            return M_PI*glm::normalize(glm::vec3(1, 0, 0) - axis[0] * axis);
        }
        else if (fabs(axis[1]) <= fabs(axis[2]) && fabs(axis[1]) <= fabs(axis[0])) {
            return M_PI*glm::normalize(glm::vec3(0, 1, 0) - axis[1] * axis);
        }
        else {
            return M_PI*glm::normalize(glm::vec3(0, 0, 1) - axis[2] * axis);
        }
    }
    else {
        return (M_PI / 2.0f)*glm::cross(axis, target);
    }
}

glm::vec3 Math::axisAngleAlignZYtoVECS3(const glm::vec3& zIn, const glm::vec3& yIn) {
    glm::vec3 z = glm::normalize(zIn);
    glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
    glm::vec3 x = glm::cross(y, z);
    glm::mat3 R = glm::mat3(x, y, z);
    return axisAngleRotation3(R);
}
glm::vec3 Math::axisAngleAlignVECStoZY3(const glm::vec3& z, const glm::vec3& y) { return -axisAngleAlignZYtoVECS3(z, y); }
AxisAngleRotation2 Math::axisAngleAlignVECStoZY2(const glm::vec3& z, const glm::vec3& y) { return axisAngleRotation2(axisAngleAlignVECStoZY3(z, y)); }
AxisAngleRotation2 Math::axisAngleAlignZYtoVECS2(const glm::vec3& z, const glm::vec3& y) { return axisAngleRotation2(axisAngleAlignZYtoVECS3(z, y)); }

glm::mat3 Math::matrixAlignVECtoZ(const glm::vec3& v) { return rotationMatrix(axisAngleAlignVECtoZ3(v)); }
glm::mat3 Math::matrixAlignZtoVEC(const glm::vec3& v) { return rotationMatrix(axisAngleAlignZtoVEC3(v)); }