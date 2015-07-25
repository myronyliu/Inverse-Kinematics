#include "Body.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

//////////////////////
//// CONSTRUCTORS ////
//////////////////////

Body::Body() :
Object(), _skeleton(NULL),
_anchoredTranslations(std::map<SkeletonComponent*, glm::vec3>()),
_anchoredRotations(std::map<SkeletonComponent*, glm::vec3>())
{}

Body::Body(Skeleton* skeleton) :
Object(), _skeleton(skeleton),
_anchoredTranslations(std::map<SkeletonComponent*, glm::vec3>()),
_anchoredRotations(std::map<SkeletonComponent*, glm::vec3>())
{}

Body::Body(Bone* bone) :
Object(), _skeleton(bone->skeleton()),
_anchoredTranslations(std::map<SkeletonComponent*, glm::vec3>()),
_anchoredRotations(std::map<SkeletonComponent*, glm::vec3>())
{}




std::set<SkeletonComponent*> Body::anchors() const {
    std::set<SkeletonComponent*> anchors;
    for (auto anchor : _anchoredTranslations)
        anchors.insert(anchor.first);
    for (auto anchor : _anchoredRotations)
        anchors.insert(anchor.first);
    return anchors;
}

void Body::anchor(SkeletonComponent* component, const bool& tFixed, const bool& wFixed) {
    if (tFixed || wFixed) {
        for (auto effector : _effectors)
            addEffector(effector.first);
    }

    if (tFixed)
        _anchoredTranslations[component] = component->globalTranslation();
    else
        _anchoredTranslations.erase(component);

    if (wFixed)
        _anchoredRotations[component] = component->globalRotation();
    else
        _anchoredRotations.erase(component);
}
void Body::unanchor(SkeletonComponent* component) {
    _anchoredTranslations.erase(component);
    _anchoredRotations.erase(component);
    for (auto effector : _effectors)
        addEffector(effector.first);
}

void Body::addEffector(SkeletonComponent* effector) {
    typedef TreeNode<SkeletonComponent*>* StandardNode;
    typedef TreeNode<StandardNode>* BranchNode;
    typedef std::vector<SkeletonComponent*> ComponentPath;

    auto makeComponentPath = [](const BranchNode& branchNode, const bool& isRoot) {
        std::vector<StandardNode> nodePath = branchNode->data()->pathToLeftMostLeaf();
        int n = nodePath.size();
        ComponentPath componentPath(n, NULL);
        for (int i = 0; i < n; i++) {
            componentPath[i] = nodePath[n - 1 - i]->data();
        }
        if (!isRoot) {
            componentPath.push_back(nodePath.front()->parent()->data());
        }
        return componentPath;
    };

    TreeNode<SkeletonComponent*>* effectorToAnchorsTree
        = effector->buildTreeToTargets(std::set<SkeletonComponent*>(anchors()));
    TreeNode<TreeNode<SkeletonComponent*>*>* branchTree = effectorToAnchorsTree->buildBranchTree();

    std::vector<BranchNode> branchNodeSeqn = branchTree->DFSsequence();

    TreeNode<ComponentPath>* root = new TreeNode<ComponentPath>(makeComponentPath(branchNodeSeqn[0], true));

    TreeNode<ComponentPath>* previousNode = root;
    int previousDepth = branchNodeSeqn[0]->depth();
    for (int i = 1; i < branchNodeSeqn.size(); i++) {
        int depth = branchNodeSeqn[i]->depth();
        if (depth > previousDepth) {
            TreeNode<ComponentPath>* newNode
                = new TreeNode<ComponentPath>(makeComponentPath(branchNodeSeqn[i], false), previousNode);
            previousNode = newNode;
        }
        else {
            previousNode = previousNode->parent();
        }
        previousDepth = depth;
    }

    _effectors[effector] = root->BFSdataSequence();

    branchTree->suicide();
    effectorToAnchorsTree->suicide();
}





void Body::doDraw() {

    if (_skeleton == NULL) return;
    if (_skeleton->bones().size() == 0) return;

    Bone* root = NULL;
    std::set<SkeletonComponent*> anchors = this->anchors();
    if (!anchors.empty()) {
        SkeletonComponent* firstAnchor = *anchors.begin();
        if (Connection* connection = dynamic_cast<Connection*>(firstAnchor)) {
            if (connection->bone() != NULL) root = connection->bone();
            else root = connection->opposingBone();
        }
        else root = dynamic_cast<Bone*>(firstAnchor);
    }
    else root = *_skeleton->bones().begin();


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
        if (true) {
            glPushMatrix();
            pushTranslation(bone->globalTranslation());
            pushRotation(bone->globalRotation());

            glPushAttrib(GL_COLOR_MATERIAL);
            if (anchors.find(bone) != anchors.end()) {
                glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
            }
            else if (_effectors.find(bone) != _effectors.end()) {
                glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
            }

            bone->draw(0.2);

            glPopAttrib();

            glPopMatrix();
        }
        if (false) {
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

void Body::hardUpdate(SkeletonComponent* rootIn) const {
    SkeletonComponent* root = rootIn;
    if (root == NULL) {
        std::set<SkeletonComponent*> anchors = this->anchors();
        if (anchors.size() == 0) {
            root = *_skeleton->bones().begin();
        }
        else {
            root = *anchors.begin();
        }
    }
    TreeNode<SkeletonComponent*>* updateTree = root->buildTreeTowards(root->connectedComponents());
    updateGlobals(updateTree);
    updateTree->suicide();
}

void Body::setTranslation(SkeletonComponent* effector, const glm::vec3& target) {
    if (_anchoredTranslations.find(effector) != _anchoredTranslations.end()) return;

    if (_effectors.find(effector) == _effectors.end())
        addEffector(effector);
 
    std::vector<ComponentPath> pathSeqn = _effectors[effector];

    linearSetIK(pathSeqn[0], target);

    for (int i = 1; i < pathSeqn.size(); i++) {
        std::vector<SkeletonComponent*> IKpath = pathSeqn[i];
        std::vector<SkeletonComponent*> updatePath({ IKpath.back(), IKpath[IKpath.size() - 2] });
        IKpath.pop_back();
        IKpath.back()->backup();
        updateGlobals(updatePath);
        glm::vec3 t = IKpath.back()->globalTranslation();
        IKpath.back()->restore();
        linearSetIK(IKpath, t);
    }

    /*std::vector<ComponentPath> pathSeqn = _effectors[effector];
    int nPaths = pathSeqn.size();

    auto backupAll = [&]() {
        for (auto componentPath: pathSeqn) {
            for (auto component : componentPath) {
                component->backup();
            }
        }
    };
    auto restoreAll = [&]() {
        for (auto componentPath : pathSeqn) {
            for (auto component : componentPath) {
                component->restore();
            }
        }
    };

    glm::vec3 effectorPosition = effector->globalTranslation();
    glm::vec3 stepToTarget = target - effectorPosition;
    float distanceToTarget = glm::length(stepToTarget);

    bool success = false;
    int maxTries = 128;
    int tries = 0;

    

    backupAll();
    while (distanceToTarget > 0.1f && tries < maxTries) {

        linearNudgeIK(pathSeqn[0], stepToTarget);

        glm::vec3 newEffectorPosition = effector->globalTranslation();
        glm::vec3 newStepToTarget = target - newEffectorPosition;
        float newDistanceToTarget = glm::length(newStepToTarget);

        if (newDistanceToTarget >= distanceToTarget) {
            restoreSkeletonComponents(pathSeqn[0]);
            stepToTarget /= 2;
            tries++;
        }
        else {
            bool anchorsViolated = false;
            for (int i = 1; i < nPaths; i++) {
                std::vector<SkeletonComponent*> IKpath = pathSeqn[i];
                std::vector<SkeletonComponent*> updatePath({ IKpath.back(), IKpath[IKpath.size() - 2] });
                IKpath.pop_back();
                IKpath.back()->backup();
                updateGlobals(updatePath);
                glm::vec3 t = IKpath.back()->globalTranslation();
                IKpath.back()->restore();
                if (!linearSetIK(IKpath, t)) {
                    restoreAll();
                    stepToTarget /= 2;
                    tries++;
                    anchorsViolated = true;
                    break;
                }
            }
            if (anchorsViolated) continue;

            backupAll();
            distanceToTarget = newDistanceToTarget;
            effectorPosition = newEffectorPosition;
            stepToTarget = newStepToTarget;
            tries = 0;
            success = true;
        }
    }*/
}