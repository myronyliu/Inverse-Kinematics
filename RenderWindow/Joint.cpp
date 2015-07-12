#include "Joint.h"

using namespace Math;

std::pair<glm::vec3, AxisAngleRotation2> Joint::tryParams(const std::vector<float>& params)  {

    glm::vec3 oldTranslation = _translation;
    AxisAngleRotation2 oldRotation = _rotation;

    backup();
    setParams(params);
    glm::vec3 translation = _translation;
    AxisAngleRotation2 rotation = _rotation;
    restore();

    if (_translation != oldTranslation || !(_rotation == oldRotation)) {
        std::cout << std::endl;
    }

    printVec3(_translation);
    _rotation.print();

    return std::pair<glm::vec3, AxisAngleRotation2>(translation, rotation);
}