#include "SkeletonComponents.h"

//////////////////////////////////////////////////////////////////////////////
//// Functions for making/breaking connections between Bone and Joint ////
//////////////////////////////////////////////////////////////////////////////

Joint* Bone::attach(Joint* joint) {
    if (joint == NULL) return NULL;
    _joints.insert(joint);
    joint->_anchor = this;
    if (joint->_target == this) {
        joint->_target = NULL;
    }
    return joint;
}
void Bone::dettach(Joint* joint) {
    _joints.erase(joint);
    joint->_anchor = NULL;
}
void Joint::attach(Bone* anchor) {
    _anchor = anchor;
    if (anchor!=NULL) anchor->_joints.insert(this);
}
void Joint::detach() {
    if (_anchor != NULL) {
        _anchor->_joints.erase(this);
        _anchor = NULL;
    }
}

///////////////////////////////////////////////////
//// Note the "duality" in the functions ABOVE ////
///////////////////////////////////////////////////

void Joint::setParams(const std::map<int, float>& params_unconstrained) {
    _params = params_unconstrained;
    constrainParams();
    buildTransformsFromParams();
}
void Joint::setParam(const int& key, const float& value) {
    _params[key] = value;
    constrainParams();
    buildTransformsFromParams();
}


bool Joint::getParam(const int& key, float& value) const {
    std::map<int, float>::const_iterator it = _params.find(key);
    if (it == _params.end()) {
        return false;
    }
    else {
        value = it->second;
        return true;
    }
}
bool Joint::getConstraint(const int& key, float& value) const {
    std::map<int, float>::const_iterator it = _constraints.find(key);
    if (it == _constraints.end()) {
        return false;
    }
    else {
        value = it->second;
        return true;
    }
}

std::map<Joint*, Bone*> Bone::jointTargets() const {
    std::map<Joint*, Bone*> map;
    for (auto joint : _joints) {
        map[joint] = joint->target();
    }
    return map;
}

Joint* Bone::getLink(Bone* neighbor) const {
    for (auto joint : _joints) {
        if (joint->target() == neighbor) {
            return joint;
        }
    }
    return NULL;
}