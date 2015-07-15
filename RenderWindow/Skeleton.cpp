#include "Skeleton.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;


void Skeleton::doDraw() {
    if (_root == NULL) return;

    _root->draw(0.2);

    vector<tuple<Bone*, Joint*, int>> stack;
    set<Bone*> drawn({ _root });

    for (auto target : _root->jointTargets()) {
        stack.push_back(tuple<Bone*, Joint*, int>(target.second, target.first, 1));
    }

    vec3 translation;
    AxisAngleRotation2 rotation;

    Bone* bone;
    Joint* joint; // the halfJoint that connects bone to the Bone it descended from in the graph-theoretic sense
    int depth;
    int previousDepth = 0; // the depth of the root, which has already been drawn
    while (stack.size() > 0) {

        tie(bone, joint, depth) = stack.back();
        stack.pop_back();


        for (auto target : bone->jointTargets()) {
            // the Bone from which "target" descended in the tree is just the variable "Bone* bone"
            if (drawn.find(target.second) == drawn.end()) {
                stack.push_back(tuple<Bone*, Joint*, int>(target.second, target.first, depth + 1));
            }
        }

        if (depth < previousDepth) {
            glPopMatrix();
            bone->draw(0.2);
        }
        else if (depth == previousDepth) {
            bone->draw(0.2);
        }
        else {
            tie(translation, rotation) = joint->alignAnchorToTarget();
            glPushMatrix();
            pushTranslation(translation);
            pushRotation(rotation);
            bone->draw(0.2);
        }
        drawn.insert(bone);
        previousDepth = depth;

    }
}