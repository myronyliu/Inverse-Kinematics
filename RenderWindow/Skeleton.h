#ifndef _SKELETON_H_
#define _SKELETON_H_

#include "Scene.h"
#include "SkeletonComponents.h"

namespace Scene {

    class Bone;
    class Joint;
    class Socket;

    class Skeleton : public Object
    {
        friend class Bone;
    public:
        Skeleton() : Object(), _root(NULL) {}
        Skeleton(Bone* root) : Object(), _root(root) {}
        template <class T>
        Skeleton(T bones) : _bones(std::set<Bone*>(bones.begin(), bones.end())) {}

        Bone* root() const { return _root; }
        void setRoot(Bone* root) { _root = root; }

        std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>> bonesSocketsJoints() const;
        std::vector<Bone*> bones() const;
        std::vector<Socket*> sockets() const;
        std::vector<Joint*> joints() const;

        void jiggle(const float& amplitude = 1);

        void doDraw();
    private:
        Bone* _root;
        std::set<Bone*> _bones;
    };

}

#endif