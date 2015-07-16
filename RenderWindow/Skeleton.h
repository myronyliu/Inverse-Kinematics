#ifndef _SKELETON_H_
#define _SKELETON_H_

#include "Scene.h"
#include "SkeletonComponents.h"

namespace Scene {


    class Skeleton : public Object
    {
    public:
        Skeleton() : Object(), _root(NULL) {}
        Skeleton(Bone* root) : Object(), _root(root) {}

        Bone* root() const { return _root; }
        void setRoot(Bone* root) { _root = root; }

        std::pair<std::vector<Bone*>, std::vector<Joint*>> bonesAndJoints() const;
        std::vector<Bone*> bones() const { return bonesAndJoints().first; }
        std::vector<Joint*> joints() const { return bonesAndJoints().second; }

        void jiggle(const float& amplitude = 1) { for (auto joint : joints()) joint->perturb(amplitude); }

        void doDraw();
    private:
        Bone* _root;

    };

}

#endif