#include "BodyComponents.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

void SkeletonComponent::setGlobalTranslation(const glm::vec3& tGlobal) {
    _tGlobal = tGlobal;
    if (Connection* connection = dynamic_cast<Connection*>(this)) {
        Bone* anchor = connection->bone();
        if (anchor != NULL) {
            glm::mat3 RGlobal = Math::R(_wGlobal);
            glm::mat3 StandardToAnchor = RGlobal*Math::R(RGlobal*(-connection->rotationFromBone()));
            anchor->_tGlobal = tGlobal + StandardToAnchor*(-connection->translationFromBone());
        }
    }
    if (Bone* bone = dynamic_cast<Bone*>(this)) {
        for (auto connection : bone->connections()) {
            connection->_tGlobal = tGlobal + Math::rotate(connection->translationFromBone(), _wGlobal);
        }
    }
}

void SkeletonComponent::setGlobalRotation(const glm::vec3& wGlobal) {
    _wGlobal = wGlobal;
    if (Connection* connection = dynamic_cast<Connection*>(this)) {
        Bone* anchor = connection->bone();
        if (anchor != NULL) {
            glm::mat3 RGlobal = Math::R(wGlobal);
            glm::mat3 StandardToAnchor = RGlobal*Math::R(RGlobal*(-connection->rotationFromBone()));
            anchor->_wGlobal = Math::w(StandardToAnchor);
        }
    }
    if (Bone* bone = dynamic_cast<Bone*>(this)) {
        for (auto connection : bone->connections()) {
            connection->_wGlobal = Math::R(wGlobal)*Math::rotate(connection->rotationFromBone(), wGlobal);
        }
    }
}

void SkeletonComponent::setGlobalTranslationAndRotation(const glm::vec3& tGlobal, const glm::vec3& wGlobal) {
    _wGlobal = wGlobal;
    _tGlobal = tGlobal;
    if (Connection* connection = dynamic_cast<Connection*>(this)) {
        Bone* anchor = connection->bone();
        if (anchor != NULL) {
            glm::mat3 RGlobal = Math::R(wGlobal);
            glm::mat3 StandardToAnchor = RGlobal*Math::R(RGlobal*(-connection->rotationFromBone()));
            anchor->_wGlobal = Math::w(StandardToAnchor);
            anchor->_tGlobal = tGlobal + StandardToAnchor*(-connection->translationFromBone());
        }
    }
    if (Bone* bone = dynamic_cast<Bone*>(this)) {
        for (auto connection : bone->connections()) {
            connection->_wGlobal = Math::R(wGlobal)*Math::rotate(connection->rotationFromBone(), wGlobal);
            connection->_tGlobal = tGlobal + Math::rotate(connection->translationFromBone(), wGlobal);
        }
    }
}