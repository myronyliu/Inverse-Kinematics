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

        void doDraw();
    private:
        Bone* _root;

    };

}

#endif