
#include "Math.h"
#include "utils.h"


glm::vec3 Math::clampRotation(const glm::vec3& w) {
    return Math::mod(glm::length(w), 2 * M_PI)*glm::normalize(w);
}

glm::vec3 Math::rotate(const glm::vec3& v, const glm::vec3& w) {
    float angle = glm::length(w);
    if (angle == 0) return v;
    else return glm::rotate(v, angle, w / angle);
}

float Math::clamp(const float& lowerBound, const float& value, const float& upperBound) {
    if (lowerBound > upperBound) return value;
    else if (value < lowerBound) return lowerBound;
    else if (value > upperBound) return upperBound;
    else return value;
}

float Math::mod(const float& value, const float& modulus) {
    return value - floor(value / modulus)*modulus;
}

float Math::periodicClamp(const float& lowerBoundIn, const float& valueIn, const float& upperBoundIn, const float& period) {
    float value = mod(valueIn, period);
    float lowerBound = mod(lowerBoundIn, period);
    float upperBound = mod(upperBoundIn, period);
    if (upperBound < lowerBound) upperBound += period;
    if (value < lowerBound) value += period;
    return mod(clamp(lowerBound, value, upperBound), period);
}

glm::mat3 Math::basisChangeMatrix(const glm::mat3& oldBasis, const glm::mat3& newBasis) {
    return glm::inverse(newBasis)*oldBasis;
}

glm::mat3 Math::changeToBasis(const glm::mat3& transformation, const glm::mat3& basis) {
    return glm::inverse(basis)*transformation*basis;
}
glm::mat3 Math::revertFromBasis(const glm::mat3& transformation, const glm::mat3& basis) {
    return basis*transformation*glm::inverse(basis);
}

glm::mat3 Math::composeLocalTransforms(const glm::mat3& A, const glm::mat3& B) {
    return revertFromBasis(B, A)*A;
}
glm::vec3 Math::composeLocalRotations(const glm::vec3& w0, const glm::vec3& w1) {
    float angle0 = glm::length(w0);
    float angle1 = glm::length(w1);
    if (angle0 == 0) return w1;
    if (angle1 == 0) return w0;
    glm::mat3 R0 = Math::R(w0);
    glm::vec3 w1_frame0 = R0*w1;
    glm::mat3 R = Math::R(w1_frame0)*R0;
    return Math::w(R);
}
AxisAngleRotation2 Math::composeLocalRotations(const AxisAngleRotation2& axisAngle0, const AxisAngleRotation2& axisAngle1) {
    glm::vec3 w = composeLocalRotations(axisAngle0.axisAngleRotation3(), axisAngle1.axisAngleRotation3());
    return AxisAngleRotation2(w);
}
glm::vec3 Math::composeLocalRotations(const std::vector<glm::vec3>& localRotations) {
    glm::mat3 R;
    for (auto w_local : localRotations) {
        glm::vec3 w_global = R*w_local;
        R = Math::R(w_global)*R;
    }
    return Math::w(R);
}
AxisAngleRotation2 Math::composeLocalRotations(const std::vector<AxisAngleRotation2>& localRotations) {
    std::vector<glm::vec3> ws(localRotations.size());
    for (int i = 0; i < localRotations.size(); i++)
        ws[i] = localRotations[i].axisAngleRotation3();
    return AxisAngleRotation2(Math::composeLocalRotations(ws));
}
std::vector<std::pair<glm::vec3, glm::vec3>>
Math::composeLocalTransforms(const std::vector<std::pair<glm::vec3, glm::vec3>>& localTransforms)
{
    int nTransforms = localTransforms.size();
    std::vector<std::pair<glm::vec3,glm::vec3>> composedTransforms(nTransforms);
    glm::vec3 t(0,0,0), t_local, w_local;
    glm::mat3 R;
    for (int i = 0; i < nTransforms; i++) {
        std::tie(t_local, w_local) = localTransforms[i];
        glm::vec3 t_global = R*t_local;
        glm::vec3 w_global = R*w_local;
        R = Math::R(w_global)*R;
        t += t_global;
        composedTransforms[i] = std::make_pair(t, Math::w(R));
    }
    return composedTransforms;
}

glm::mat3 Math::R(const glm::vec3& w) {

    float theta = glm::length(w);
    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta == 0) return glm::mat3();
    glm::vec3 wHat = glm::normalize(w);
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
glm::mat3 Math::R(const AxisAngleRotation2& axisAngle) {
    return Math::R(axisAngle.axisAngleRotation3());
}
glm::mat3 Math::R(const glm::vec2& axis, const float& angle) {
    return Math::R(Math::w(axis, angle));
}
glm::mat3 Math::R(const float& angle, const glm::vec2& axis) {
    return Math::R(Math::w(axis, angle));
}


glm::vec3 Math::w(const glm::mat3& R) {
    float acosArg = (R[0][0] + R[1][1] + R[2][2] - 1) / 2;
    if (acosArg < -0.99999999) {
        if (R[0][0] >= R[1][1] && R[0][0] >= R[2][2]) {
            float x = sqrt((R[0][0] + 1) / 2);
            return M_PI*glm::vec3(x, R[1][0] / (2 * x), R[2][0] / (2 * x));
        }
        else if (R[1][1] >= R[2][2] && R[1][1] >= R[0][0]) {
            float y = sqrt((R[1][1] + 1) / 2);
            return M_PI*glm::vec3(R[0][1] / (2 * y), y, R[2][1] / (2 * y));
        }
        else {
            float z = sqrt((R[2][2] + 1) / 2);
            return M_PI*glm::vec3(R[0][2] / (2 * z), R[1][2] / (2 * z), z);
        }
    }
    else if (acosArg > 0.99999999) {
        return glm::vec3(0, 0, 0);
    }
    else{
        float theta = acos(acosArg);
        return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
    }
}
AxisAngleRotation2 Math::axisAngleRotation2(const glm::mat3& R) {
    return AxisAngleRotation2(Math::w(R));
}

AxisAngleRotation2 Math::axisAngleRotation2(const glm::vec3& w) {
    float angle = glm::length(w);
    if (angle == 0) return AxisAngleRotation2(glm::vec2(0, 0), 0);
    glm::vec2 axis(acos(w[2] / angle), atan2(w[1], w[0]));
    angle = mod(angle, 2 * M_PI);
    return AxisAngleRotation2(axis, angle);
}


glm::vec3 Math::w(const glm::vec2& axis, const float& angle) {
    return mod(angle, 2 * M_PI)*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}
glm::vec3 Math::w(const float& angle, const glm::vec2& axis) {
    return mod(angle, 2 * M_PI)*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}
glm::vec3 Math::w(const AxisAngleRotation2& axisAngle) {
    glm::vec2 axis = axisAngle._axis;
    float angle = mod(axisAngle._angle, 2 * M_PI);
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}

glm::vec3 Math::axisAngleAlignVECtoZ3(const glm::vec3& zIn) {
    glm::vec3 z = glm::normalize(zIn);
    if (z[0] == 0 && z[1] == 0) {
        if (z[2] >= 0) return glm::vec3(0, 0, 0);
        else return glm::vec3(M_PI, 0, 0);
    }
    else {
        glm::vec3 axis = glm::cross(z, glm::vec3(0, 0, 1));
        float sinAngle = clamp(0.0f, glm::length(axis), 1.0f);
        float angle = asin(sinAngle);
        if (z[2] < 0) angle = M_PI - angle;
        return angle*glm::normalize(axis);
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
    return Math::w(R);
}
glm::vec3 Math::axisAngleAlignVECStoZY3(const glm::vec3& z, const glm::vec3& y) { return -axisAngleAlignZYtoVECS3(z, y); }
AxisAngleRotation2 Math::axisAngleAlignVECStoZY2(const glm::vec3& z, const glm::vec3& y) { return axisAngleRotation2(axisAngleAlignVECStoZY3(z, y)); }
AxisAngleRotation2 Math::axisAngleAlignZYtoVECS2(const glm::vec3& z, const glm::vec3& y) { return axisAngleRotation2(axisAngleAlignZYtoVECS3(z, y)); }

glm::mat3 Math::matrixAlignVECtoZ(const glm::vec3& v) { return Math::R(axisAngleAlignVECtoZ3(v)); }
glm::mat3 Math::matrixAlignZtoVEC(const glm::vec3& v) { return Math::R(axisAngleAlignZtoVEC3(v)); }