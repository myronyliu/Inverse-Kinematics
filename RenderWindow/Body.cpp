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

    _effectors[effector] = root;

    branchTree->suicide();
    effectorToAnchorsTree->suicide();
}





void Body::doDraw() {

    if (_skeleton == NULL) return;
    if (_skeleton->bones().size() == 0) return;

    std::set<SkeletonComponent*> anchors = this->anchors();
    if (anchors.size() == 0) return;
    SkeletonComponent* firstAnchor = *anchors.begin();
    Bone* root = NULL;
    if (Connection* connection = dynamic_cast<Connection*>(firstAnchor)) {
        if (connection->bone() != NULL) root = connection->bone();
        else root = connection->opposingBone();
    }
    else root = dynamic_cast<Bone*>(firstAnchor);


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
            bone->draw(0.2);
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

void Body::setTranslation(SkeletonComponent* effector, const glm::vec3& t) {
    if (_anchoredTranslations.find(effector) != _anchoredTranslations.end()) return;

    if (_effectors.find(effector) == _effectors.end())
        addEffector(effector);

    glm::vec3 displacement = t - effector->globalTranslation();
    float distance = glm::length(displacement);
    
    if (distance < 0.000001f) return;
    if (distance > 0.0001f) displacement *= 0.0001f / distance;
 
    TreeNode<std::vector<SkeletonComponent*>>* IKbranchTree = _effectors[effector];
    std::vector<TreeNode<std::vector<SkeletonComponent*>>*> IKbranchSeqn = IKbranchTree->BFSsequence();

    linearIK(IKbranchSeqn[0]->data(), t);

    /*for (int i = 1; i < IKbranchSeqn.size(); i++) {
        std::vector<SkeletonComponent*> IKbranch = IKbranchSeqn[i]->data();
        glm::vec3 t = IKbranch.back()->globalTranslation();
        glm::vec3 w = IKbranch.back()->globalRotation();
        std::vector<SkeletonComponent*> updatePath({ IKbranch[IKbranch.size() - 2], IKbranch.back() });
        updateGlobals(updatePath);
        linearIK(IKbranch, t, w);
    }*/

    for (int i = 1; i < IKbranchSeqn.size(); i++) {
        std::vector<SkeletonComponent*> IKbranch = IKbranchSeqn[i]->data();
        std::vector<SkeletonComponent*> updatePath({ IKbranch.back(), IKbranch[IKbranch.size() - 2] });
        IKbranch.pop_back();
        IKbranch.back()->backup();
        updateGlobals(updatePath);
        glm::vec3 t = IKbranch.back()->globalTranslation();
        IKbranch.back()->restore();
        linearIK(IKbranch, t);
    }
}