#ifndef _BODY_H_
#define _BODY_H_

#include "stdafx.h"
#include "Rotation.h"
#include "Joint.h"
#include "utils.h"

class Joint;


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
    void attach(Joint* joint) {
        _joints.insert(joint);
    }
    void dettach(Joint* joint) {
        _joints.erase(joint);
    }
    std::set<Joint*> joints() const {
        return _joints;
    }
private:
    // The following are relative to some global system
    // I don't think these are really necessary since it can be reclaimed by traversing the joints elsewhere all the way to this Body
    // However, this is convenient as it will serve the function of _globalRotations and _globalTranslations that was once in Scene::Arm
    glm::vec3 _translation;
    AxisAngleRotation2 _rotation;

    std::set<Joint*> _joints;
};

#endif