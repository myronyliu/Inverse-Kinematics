#ifndef _BODY_H_
#define _BODY_H_

#include "Scene.h"
#include "BodyComponents.h"

namespace Scene {

    class Bone;
    class Joint;
    class Socket;
    class Skeleton;

    class Body : public Object
    {
    public:
        Body() : Object() {}
        Body(Skeleton* skeleton) : Object(), _skeleton(skeleton) {}
        Body(Bone* bone) : Object(), _skeleton(bone->skeleton()) {}

        void anchor(Bone* bone) { _anchors.insert(bone); }
        void unanchor(Bone* bone) { _anchors.erase(bone); }

        std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>> bonesSocketsJoints() const;
        std::vector<Bone*> bones() const;
        std::vector<Socket*> sockets() const;
        std::vector<Joint*> joints() const;
        Skeleton* skeleton() const { return _skeleton; }

        void doDraw();
    private:
        std::set<Bone*> _anchors;
        Skeleton* _skeleton;
    };

}

#endif