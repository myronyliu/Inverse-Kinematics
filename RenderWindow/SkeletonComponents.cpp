#include "SkeletonComponents.h"

//////////////////////////////////////////////////////////////////////////////
//// Functions for making/breaking connections between Bone and Joint ////
//////////////////////////////////////////////////////////////////////////////

Joint* Bone::attach(Joint* joint) {
    if (joint == NULL) return NULL;
    _anchoredJoints.insert(joint);
    joint->_anchor = this;
    if (joint->_target == this) {
        joint->_target = NULL;
        _targetingJoints.erase(joint);
    }
    return joint;
}
void Bone::unattach(Joint* joint) {
    _anchoredJoints.erase(joint);
    joint->_anchor = NULL;
}
Joint* Bone::couple(Joint* joint) {
    if (joint == NULL) return NULL;
    _targetingJoints.insert(joint);
    joint->_target = this;
    if (joint->_anchor == this) {
        joint->_anchor = NULL;
        _anchoredJoints.erase(joint);
    }
    return joint;
}
void Bone::uncouple(Joint* joint) {
    _targetingJoints.erase(joint);
    joint->_target = NULL;
}


void Joint::attach(Bone* anchor) {
    
    if (anchor == _anchor) return;

    if (anchor != NULL) {
        anchor->_anchoredJoints.insert(this);
        if (anchor == _target) {
            _target->_targetingJoints.erase(this);
            _target = NULL;
        }
    }

    if (_anchor != NULL) {
        _anchor->_anchoredJoints.erase(this);
    }

    _anchor = anchor;
}
void Joint::unattach() {
    if (_anchor != NULL) {
        _anchor->_anchoredJoints.erase(this);
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

std::map<Joint*, Bone*> Bone::coupledBones() const {
    std::map<Joint*, Bone*> map;
    for (auto joint : _anchoredJoints) {
        map[joint] = joint->target();
    }
    return map;
}

std::map<Joint*, Bone*> Bone::attachingBones() const {
    std::map<Joint*, Bone*> map;
    for (auto joint : _targetingJoints) {
        map[joint] = joint->anchor();
    }
    return map;
}

Joint* Bone::getLink(Bone* neighbor) const {
    for (auto joint : _anchoredJoints) {
        if (joint->target() == neighbor) {
            return joint;
        }
    }
    return NULL;
}