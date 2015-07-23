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

void Skeleton::updateGlobals(TreeNode<SkeletonComponent*>* componentTree) {
    std::vector<TreeNode<SkeletonComponent*>*> seqn = componentTree->DFSsequence();

    SkeletonComponent* root = seqn[0]->data();
    TransformStack transformStack(root->globalTranslation(), root->globalRotation());


    for (int i = 1; i < seqn.size(); i++) {
        SkeletonComponent* component = seqn[i]->data();

        int depth = seqn[i]->depth();
        int previousDepth = seqn[i-1]->depth();

        if (depth < previousDepth) {
            transformStack.pop();
        }
        else if (depth == previousDepth) {
            std::cout << std::endl; // this should never happen
        }
        else {
            glm::vec3 t, w;
            SkeletonComponent* previousComponent = seqn[i - 1]->data();
            std::tie(t, w) = previousComponent->transformsToConnectedComponents()[component];

            transformStack.push();
            transformStack.translate(t);
            transformStack.rotate(w);

            component->setGlobalTranslation(transformStack.getTranslation());
            component->setGlobalRotation(transformStack.getRotation());
        }
    }
}
