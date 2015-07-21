#include "Body.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

void anchor(Bone* bone);
void unanchor(Bone* bone);

void Body::doDraw() {

    if (_skeleton == NULL) return;
    if (_skeleton->bones().size() == 0) return;

    Bone* root;
    if (_anchoredBones.empty()) root = *_skeleton->bones().begin();
    else root = *_anchoredBones.begin();

    int nPush = 0;
    int nPop = 0;

    vector<tuple<Bone*, Connection*, int>> stack;
    stack.push_back(tuple<Bone*, Connection*, int>(root, NULL, 0));
    set<Bone*> drawn;
    vector<bool> depthVisited;

    for (auto target : root->connectionToBones()) {
        if (target.second != NULL) {
            stack.push_back(make_tuple(target.second, target.first, 1));
        }
    }

    vec3 translation;
    vec3 rotation;

    Bone* bone;
    Connection* connection; // the halfJoint that connects bone to the Bone it descended from in the graph-theoretic sense
    int depth;
    int previousDepth = 0; // the depth of the root, which is the starting point
    while (stack.size() > 0) {

        tie(bone, connection, depth) = stack.back();
        if (depthVisited.size() < depth + 1) {
            depthVisited.resize(depth + 1, false);
        }
        depthVisited[depth] = true;
        stack.pop_back();

        if (depth == 2) {
            int j = 0;
        }

        for (auto target : bone->connectionToBones()) {
            // the Bone from which "target" descended in the tree is just the variable "Bone* bone"
            if (target.second != NULL && drawn.find(target.second) == drawn.end()) {
                stack.push_back(make_tuple(target.second, target.first, depth + 1));
            }
        }
        if (false) {
            glPushMatrix();
            pushTranslation(bone->globalTranslation());
            pushRotation(bone->globalRotation());
            bone->draw(0.2);
            glPopMatrix();
        }
        if (true) {
            if (depth < previousDepth) {
                for (int i = 0; i < previousDepth - depth; i++) {
                    glPopMatrix();
                    nPop++;
                }
                if (depthVisited[depth] && bone != root) {
                    connection->transformAnchorToTarget(translation, rotation);

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
                connection->transformAnchorToTarget(translation, rotation);
                glPushMatrix();
                pushTranslation(translation);
                pushRotation(rotation);
                bone->draw(0.2);
                nPush++;
            }
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

void Body::hardUpdate() {
    Bone* anchor = *_anchoredBones.begin();
    for (auto connection : anchor->connections()) {
        TreeNode<Bone*>* boneTree = connection->boneTree();
        _skeleton->updateGlobals(boneTree);
        delete boneTree;
    }
}