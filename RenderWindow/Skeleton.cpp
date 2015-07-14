#include "Skeleton.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;


void Skeleton::doDraw() {
    if (_root == NULL) return;

    vector<tuple<Bone*, HalfJoint*, int>> stack({ tuple<Bone*, HalfJoint*, int>(_root, NULL, 0) });

    set<Bone*> drawn;

    for (auto halfJoint : _root->halfJoints()) {
        stack.push_back(tuple<Bone*, HalfJoint*, int>(halfJoint->anchor(), halfJoint->partner(), 1));
    }

    vec3 translation;
    AxisAngleRotation2 rotation;

    Bone* bone;
    HalfJoint* edge; // the halfJoint that connects bone to the Bone it descended from in the graph-theoretic sense
    int depth;
    int previousDepth = 0;
    while (stack.size() > 0) {
        tie(bone, edge, depth) = stack.back();
        stack.pop_back();

        for (auto halfJoint : bone->halfJoints()) {
            Bone* neighbor = halfJoint->anchor();
            HalfJoint* halfJointToPrevious = halfJoint->partner();
            if (drawn.find(neighbor) != drawn.end()) {
                stack.push_back(tuple<Bone*, HalfJoint*, int>(neighbor, halfJointToPrevious, depth + 1));
            }
        }

        if (depth < previousDepth) {
            glPopMatrix();
            bone->draw();
        }
        else if (depth == previousDepth) {
            bone->draw();
        }
        else {
            tie(translation, rotation) = edge->alignAnchorToTarget();
            glPushMatrix();
            pushTranslation(translation);
            pushRotation(rotation);
            bone->draw();
        }
        drawn.insert(bone);
    }

    glPopMatrix();
}