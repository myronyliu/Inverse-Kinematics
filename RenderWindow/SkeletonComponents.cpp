#include "SkeletonComponents.h"

std::map<HalfJoint*, Bone*> Bone::neighbors() const {
    std::map<HalfJoint*, Bone*> map;
    for (auto halfJoint : _halfJoints) {
        HalfJoint* asdf = halfJoint;
        asdf->partner();
        halfJoint->partner();
        map[halfJoint] = halfJoint->partner()->anchor();
    }
    return map;
}

HalfJoint* Bone::getLink(Bone* neighbor) const {
    for (auto halfJoint : _halfJoints) {
        if (halfJoint->partner()->anchor() == neighbor) {
            return halfJoint;
        }
    }
    return NULL;
}