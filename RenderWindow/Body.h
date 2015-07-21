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
        Body() : Object(), _skeleton(NULL), _anchoredBones(std::set<Bone*>({})) {}
        Body(Skeleton* skeleton) : Object(), _skeleton(skeleton), _anchoredBones(std::set<Bone*>({})) {}
        Body(Bone* bone) : Object(), _skeleton(bone->skeleton()), _anchoredBones(std::set<Bone*>({})) {}

        void anchor(Bone* bone) { _anchoredBones.insert(bone); }
        void unanchor(Bone* bone) { _anchoredBones.erase(bone); }

        std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>> bonesSocketsJoints() const;
        std::vector<Bone*> bones() const;
        std::vector<Socket*> sockets() const;
        std::vector<Joint*> joints() const;
        Skeleton* skeleton() const { return _skeleton; }

        void hardUpdate();
        void jiggle(const float& magnitude = 1) { _skeleton->jiggle(magnitude); hardUpdate(); }

        void doDraw();
    private:
        std::set<Bone*> _anchoredBones;
        Skeleton* _skeleton;
    };

}

#endif