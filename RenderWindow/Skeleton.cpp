#include "Skeleton.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;


void Skeleton::doDraw() {
    if (_root == NULL) return;

    _root->draw(0.2);

    vector<tuple<Bone*, HalfJoint*, int>> stack;
    set<Bone*> drawn({ _root });

    for (auto halfJoint : _root->halfJoints()) {
        Bone* neighbor = halfJoint->anchorBone();
        if (neighbor != NULL) {
            stack.push_back(tuple<Bone*, HalfJoint*, int>(neighbor, halfJoint->opposingHalfJoint(), 1));
        }
    }

    vec3 translation;
    AxisAngleRotation2 rotation;

    Bone* bone;
    HalfJoint* edge; // the halfJoint that connects bone to the Bone it descended from in the graph-theoretic sense
    int depth;
    int previousDepth = 0; // the depth of the root, which has already been drawn
    while (stack.size() > 0) {

        tie(bone, edge, depth) = stack.back();
        stack.pop_back();


        for (auto halfJoint : bone->halfJoints()) {
            Bone* neighbor = halfJoint->anchorBone();
            HalfJoint* halfJointToCurrent = halfJoint->opposingHalfJoint();
            // ^ the halfJoint on the neighbor that points back to this Bone
            if (neighbor != NULL && drawn.find(neighbor) == drawn.end()) {
                stack.push_back(tuple<Bone*, HalfJoint*, int>(neighbor, halfJointToCurrent, depth + 1));
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
            tie(translation, rotation) = edge->alignAnchorToTarget();
            glPushMatrix();
            pushTranslation(translation);
            pushRotation(rotation);
            bone->draw(0.2);
        }
        drawn.insert(bone);
        previousDepth = depth;

    }
}