#include "BodyComponents.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;

std::set<std::pair<Socket*, Joint*>> Skeleton::socketJoints() const {
    if (_bones.size() == 0) return std::set<std::pair<Socket*, Joint*>>();
    
    Bone* root = *_bones.begin();

    std::set<std::pair<Socket*, Joint*>> socketJoints;
    std::set<Bone*> visitedBones({ root });
    std::vector<Bone*> boneStack({ root });

    do {
        Bone* bone = boneStack.back();
        boneStack.pop_back();

        for (auto connection : bone->connections()) {
            Bone* opposingBone = connection->opposingBone();
            if (opposingBone == NULL) continue;
            socketJoints.insert(connection->socketJoint());
            if (visitedBones.find(opposingBone) == visitedBones.end()) {
                visitedBones.insert(opposingBone);
                boneStack.push_back(opposingBone);
            }
        }
    } while (boneStack.size() > 0);

    return socketJoints;
}

std::set<Socket*> Skeleton::sockets() const {
    set<Socket*> sockets;
    for (auto socketJoint : socketJoints())
        sockets.insert(socketJoint.first);
    return sockets;
}
std::set<Joint*> Skeleton::joints() const {
    set<Joint*> joints;
    for (auto socketJoint : socketJoints())
        joints.insert(socketJoint.second);
    return joints;
}
