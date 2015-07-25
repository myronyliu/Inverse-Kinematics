#ifndef _BODY_H_
#define _BODY_H_

#include "Scene.h"
#include "BodyComponents.h"

namespace Scene {

    class Bone;
    class Joint;
    class Socket;
    class Skeleton;

    typedef std::vector<SkeletonComponent*> ComponentPath;

    class Body : public Object
    {
    public:
        Body();
        Body(Skeleton* skeleton);
        Body(Bone* bone);

        void anchor(SkeletonComponent*, const bool& = true, const bool& = false);
        void unanchor(SkeletonComponent*);

        void addEffector(SkeletonComponent*);

        std::set<SkeletonComponent*> anchors() const;

        Skeleton* skeleton() const { return _skeleton; }

        void hardUpdate(SkeletonComponent* root = NULL) const;
        void jiggle(const float& magnitude = 1) { _skeleton->jiggle(magnitude); hardUpdate(); }

        void setTranslation(SkeletonComponent* component, const glm::vec3& t);

        void doDraw();
    private:
        std::map<SkeletonComponent*, glm::vec3> _anchoredTranslations;
        std::map<SkeletonComponent*, glm::vec3> _anchoredRotations;
        Skeleton* _skeleton;

        std::map<SkeletonComponent*, std::vector<ComponentPath>> _effectors;

        const glm::vec3 _t = glm::vec3(0, 0, 0);
        const glm::vec3 _w = glm::vec3(0, 0, 0);
    };

}

#endif