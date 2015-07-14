#include "SkeletonComponents.h"

//////////////////////////////////////////////////////////////////////////////
//// Functions for making/breaking connections between Bone and HalfJoint ////
//////////////////////////////////////////////////////////////////////////////

void Bone::attach(HalfJoint* halfJoint) {
    if (halfJoint == NULL) return;
    _halfJoints.insert(halfJoint);
    halfJoint->_targetBone = this;
}
void Bone::dettach(HalfJoint* halfJoint) {
    _halfJoints.erase(halfJoint);
    halfJoint->_targetBone = NULL;
}
void HalfJoint::setTargetBone(Bone* targetBone) {
    if (targetBone != NULL)targetBone->attach(this);
}
void HalfJoint::attach(Bone* targetBone) {
    if (targetBone != NULL)targetBone->attach(this);
}

///////////////////////////////////////////////////
//// Note the "duality" in the functions ABOVE ////
///////////////////////////////////////////////////


void Bone::draw(const float& scale) const {
    
    doDraw();
    for (auto halfJoint : _halfJoints) {
        halfJoint->draw(0.1);
    }
}



std::map<HalfJoint*, Bone*> Bone::neighbors() const {
    std::map<HalfJoint*, Bone*> map;
    for (auto halfJoint : _halfJoints) {
        halfJoint->opposingHalfJoint();
        map[halfJoint] = halfJoint->opposingHalfJoint()->anchorBone();
    }
    return map;
}

HalfJoint* Bone::getLink(Bone* neighbor) const {
    for (auto halfJoint : _halfJoints) {
        if (halfJoint->opposingHalfJoint()->anchorBone() == neighbor) {
            return halfJoint;
        }
    }
    return NULL;
}