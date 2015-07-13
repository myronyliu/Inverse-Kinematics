#ifndef _BODY_H_
#define _BODY_H_

#include "stdafx.h"
#include "Rotation.h"
#include "HalfJoint.h"
#include "utils.h"

class HalfJoint;

class Body
{
public:
    Body() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2()) {}
    void draw(const float& scale = 1) const {
        glPushMatrix();
        pushTranslation(_translation);
        pushRotation(_rotation);
        doDraw(scale);
        glPopMatrix();
    }
    virtual void doDraw(const float& scale = 1) const {
        GlutDraw::drawSphere(glm::vec3(0, 0, 0), scale);
    }
    void attach(HalfJoint* halfJoint) {
        _halfJoints.insert(halfJoint);
    }
    void dettach(HalfJoint* halfJoint) {
        _halfJoints.erase(halfJoint);
    }
    std::set<HalfJoint*> halfJoints() const {
        return _halfJoints;
    }
private:
    // The following are relative to some global system
    // I don't think these are really necessary since it can be reclaimed by traversing the joints elsewhere all the way to this Body
    // However, this is convenient as it will serve the function of _globalRotations and _globalTranslations that was once in Scene::Arm
    glm::vec3 _translation;
    AxisAngleRotation2 _rotation;

    std::set<HalfJoint*> _halfJoints;   // Moving around a "non-cyclic" skeleton of bodies and joints amounts to tree traversal
                                        // Starting from any root Body, access _halfJoints[i].partner().anchor() to get the ...
                                        // ... bodies adjacent to this one
};

#endif