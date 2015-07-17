#include "Skeleton.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;



std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>> Skeleton::bonesSocketsJoints() const {
    if (_root == NULL) return std::tuple<std::vector<Bone*>, std::vector<Socket*>, std::vector<Joint*>>();

    std::set<Bone*> visitedBones({ _root });
    std::set<Joint*> visitedJoints;
    std::set<Socket*> visitedSockets;
    std::vector<Bone*> boneStack({ _root });

    do {
        Bone* bone = boneStack.back();
        boneStack.pop_back();

        for (auto socket : bone->sockets()) {
            visitedSockets.insert(socket);
            Bone* opposingBone = socket->opposingBone();
            if (opposingBone == NULL) continue;
            if (visitedBones.find(opposingBone) == visitedBones.end()) {
                visitedBones.insert(opposingBone);
                boneStack.push_back(opposingBone);
            }
        }
        for (auto joint : bone->joints()) {
            visitedJoints.insert(joint);
            Bone* opposingBone = joint->opposingBone();
            if (opposingBone == NULL) continue;
            if (visitedBones.find(opposingBone) == visitedBones.end()) {
                visitedBones.insert(opposingBone);
                boneStack.push_back(opposingBone);
            }
        }
    } while (boneStack.size() > 0);

    std::vector<Bone*> bones(visitedBones.begin(), visitedBones.end());
    std::vector<Socket*> sockets(visitedSockets.begin(), visitedSockets.end());
    std::vector<Joint*> joints(visitedJoints.begin(), visitedJoints.end());

    return make_tuple(bones, sockets, joints);
}
std::vector<Bone*> Skeleton::bones() const {
    std::vector<Bone*> bones;
    std::tie(bones, std::ignore, std::ignore) = bonesSocketsJoints();
    return bones;
}
std::vector<Socket*> Skeleton::sockets() const {
    std::vector<Socket*> sockets;
    std::tie(std::ignore, sockets, std::ignore) = bonesSocketsJoints();
    return sockets;
}
std::vector<Joint*> Skeleton::joints() const {
    std::vector<Joint*> joints;
    std::tie(std::ignore, std::ignore, joints) = bonesSocketsJoints();
    return joints;
}


void Skeleton::doDraw() {
    if (_root == NULL) return;

    int nPush = 0;
    int nPop = 0;

    vector<tuple<Bone*, Socket*, int>> stack;
    stack.push_back(tuple<Bone*, Socket*, int>(_root, NULL, 0));
    set<Bone*> drawn;
    vector<bool> depthVisited;

    for (auto target : _root->socketToBones()) {
        if (target.second != NULL) {
            stack.push_back(tuple<Bone*, Socket*, int>(target.second, target.first, 1));
        }
    }

    vec3 translation;
    AxisAngleRotation2 rotation;

    Bone* bone;
    Socket* joint; // the halfJoint that connects bone to the Bone it descended from in the graph-theoretic sense
    int depth;
    int previousDepth = 0; // the depth of the root, which is the starting point
    while (stack.size() > 0) {

        tie(bone, joint, depth) = stack.back();
        if (depthVisited.size() < depth + 1) {
            depthVisited.resize(depth + 1, false);
        }
        depthVisited[depth] = true;
        stack.pop_back();
        
        
        for (auto target : bone->socketToBones()) {
            // the Bone from which "target" descended in the tree is just the variable "Bone* bone"
            if (target.second != NULL && drawn.find(target.second) == drawn.end()) {
                stack.push_back(tuple<Bone*, Socket*, int>(target.second, target.first, depth + 1));
            }
        }

        if (depth < previousDepth) {
            for (int i = 0; i < previousDepth - depth; i++) {
                glPopMatrix();
                nPop++;
            }
            if (depthVisited[depth] && bone != _root) {
                tie(translation, rotation) = joint->alignSocketedBoneToJointedBone();

                glPopMatrix();
                glPushMatrix();
                pushTranslation(translation);
                pushRotation(rotation);
            }
            bone->draw(0.2);
        }
        else {
            if (depth == previousDepth) {
                glPopMatrix();
                nPop++;
            }
            tie(translation, rotation) = joint->alignSocketedBoneToJointedBone();
            glPushMatrix();
            pushTranslation(translation);
            pushRotation(rotation);
            bone->draw(0.2);
            nPush++;
        }
        drawn.insert(bone);
        previousDepth = depth;

    }
    if (glGetError() != GL_NO_ERROR) {
        std::cout << gluErrorString(glGetError()) << std::endl;
    }
    if (nPush != nPop) {
        int j = 0;
    }
}