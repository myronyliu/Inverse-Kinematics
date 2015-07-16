#include "Skeleton.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;



std::pair<std::vector<Bone*>, std::vector<Joint*>> Skeleton::bonesAndJoints() const {
    if (_root == NULL) return std::pair<std::vector<Bone*>, std::vector<Joint*>>();

    std::set<Bone*> visitedBones({ _root });
    std::set<Joint*> visitedJoints;
    std::vector<Bone*> boneStack({ _root });

    do {
        Bone* bone = boneStack.back();
        boneStack.pop_back();

        for (auto joint : bone->joints()) {
            if (visitedJoints.find(joint) == visitedJoints.end()) {
                visitedJoints.insert(joint);
            }
            if (visitedBones.find(joint->target()) == visitedBones.end()) {
                visitedBones.insert(joint->target());
                boneStack.push_back(joint->target());
            }
        }
    } while (boneStack.size() > 0);

    std::vector<Bone*> bones(visitedBones.begin(), visitedBones.end());
    std::vector<Joint*> joints(visitedJoints.begin(), visitedJoints.end());

    return std::pair<std::vector<Bone*>, std::vector<Joint*>>(bones, joints);
}


void Skeleton::doDraw() {
    if (_root == NULL) return;

    int nPush = 0;
    int nPop = 0;

    //_root->draw(0.2);

    vector<tuple<Bone*, Joint*, int>> stack;
    stack.push_back(tuple<Bone*, Joint*, int>(_root, NULL, 0));
    set<Bone*> drawn;

    for (auto target : _root->jointTargets()) {
        if (target.second != NULL) {
            stack.push_back(tuple<Bone*, Joint*, int>(target.second, target.first, 1));
        }
    }

    vec3 translation;
    AxisAngleRotation2 rotation;

    Bone* bone;
    Joint* joint; // the halfJoint that connects bone to the Bone it descended from in the graph-theoretic sense
    int depth;
    int previousDepth = 0; // the depth of the root, which is the starting point
    while (stack.size() > 0) {

        tie(bone, joint, depth) = stack.back();
        stack.pop_back();
        
        
        for (auto target : bone->jointTargets()) {
            // the Bone from which "target" descended in the tree is just the variable "Bone* bone"
            if (target.second != NULL && drawn.find(target.second) == drawn.end()) {
                stack.push_back(tuple<Bone*, Joint*, int>(target.second, target.first, depth + 1));
            }
        }

        if (depth < previousDepth) {
            for (int i = 0; i < previousDepth - depth; i++) {
                glPopMatrix();
                nPop++;
            }
            bone->draw(0.2);
        }
        else {
            if (depth == previousDepth) {
                glPopMatrix();
                nPop++;
            }
            tie(translation, rotation) = joint->alignAnchorToTarget();
            glPushMatrix();
            pushTranslation(translation);
            pushRotation(rotation);
            bone->draw(0.2);
            nPush++;
        }
        drawn.insert(bone);
        previousDepth = depth;

    }
    if (nPush != nPop) {
        int j = 0;
    }
}