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

        std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>> bonesSocketsJoints() const;
        std::vector<Bone*> bones() const;
        std::vector<Socket*> sockets() const;
        std::vector<Joint*> joints() const;

        void jiggle(const float& amplitude = 1) { for (auto socket : sockets()) socket->perturbJoint(amplitude); }

        void doDraw();
    private:
        Bone* _root;

    };

}

#endif