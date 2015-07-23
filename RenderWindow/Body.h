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
        Body();
        Body(Skeleton* skeleton);
        Body(Bone* bone);

        void anchor(SkeletonComponent*, const bool& = true, const bool& = false);
        void unanchor(SkeletonComponent*);

        std::set<SkeletonComponent*> anchors() const;

        std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>> bonesSocketsJoints() const;
        std::vector<Bone*> bones() const;
        std::vector<Socket*> sockets() const;
        std::vector<Joint*> joints() const;
        Skeleton* skeleton() const { return _skeleton; }

        void hardUpdate() const;
        void jiggle(const float& magnitude = 1) { _skeleton->jiggle(magnitude); hardUpdate(); }

        void setTranslation(SkeletonComponent* component, const glm::vec3& t);

        void doDraw();
    private:
        std::map<SkeletonComponent*, glm::vec3> _anchoredTranslations;
        std::map<SkeletonComponent*, glm::vec3> _anchoredRotations;
        Skeleton* _skeleton;

        const glm::vec3 _t = glm::vec3(0, 0, 0);
        const glm::vec3 _w = glm::vec3(0, 0, 0);

        std::vector<SkeletonComponent*> _path;
        bool _pathReady = false;
    };

}

#endif