#include "BodyComponents.h"

using namespace std;
using namespace glm;
using namespace Math;
using namespace Scene;

void SkeletonComponent::setGlobalTranslation(const glm::vec3& tGlobal) {
    _tGlobal = tGlobal;
    if (Connection* connection = dynamic_cast<Connection*>(this)) {
        Bone* anchor = connection->bone();
        if (anchor != NULL) {
            glm::mat3 RGlobal = Math::R(_wGlobal);
            glm::mat3 StandardToAnchor = RGlobal*Math::R(RGlobal*(-connection->rotationFromBone()));
            anchor->_tGlobal = tGlobal + StandardToAnchor*(-connection->translationFromBone());
        }
    }
    if (Bone* bone = dynamic_cast<Bone*>(this)) {
        for (auto connection : bone->connections()) {
            connection->_tGlobal = tGlobal + Math::rotate(connection->translationFromBone(), _wGlobal);
        }
    }
}

void SkeletonComponent::setGlobalRotation(const glm::vec3& wGlobal) {
    _wGlobal = wGlobal;
    if (Connection* connection = dynamic_cast<Connection*>(this)) {
        Bone* anchor = connection->bone();
        if (anchor != NULL) {
            glm::mat3 RGlobal = Math::R(wGlobal);
            glm::mat3 StandardToAnchor = RGlobal*Math::R(RGlobal*(-connection->rotationFromBone()));
            anchor->_wGlobal = Math::w(StandardToAnchor);
        }
    }
    if (Bone* bone = dynamic_cast<Bone*>(this)) {
        for (auto connection : bone->connections()) {
            connection->_wGlobal = Math::R(wGlobal)*Math::rotate(connection->rotationFromBone(), wGlobal);
        }
    }
}

void SkeletonComponent::setGlobalTranslationAndRotation(const glm::vec3& tGlobal, const glm::vec3& wGlobal) {
    _wGlobal = wGlobal;
    _tGlobal = tGlobal;
    if (Connection* connection = dynamic_cast<Connection*>(this)) {
        Bone* anchor = connection->bone();
        if (anchor != NULL) {
            glm::mat3 RGlobal = Math::R(wGlobal);
            glm::mat3 StandardToAnchor = RGlobal*Math::R(RGlobal*(-connection->rotationFromBone()));
            anchor->_wGlobal = Math::w(StandardToAnchor);
            anchor->_tGlobal = tGlobal + StandardToAnchor*(-connection->translationFromBone());
        }
    }
    if (Bone* bone = dynamic_cast<Bone*>(this)) {
        for (auto connection : bone->connections()) {
            connection->_wGlobal = Math::R(wGlobal)*Math::rotate(connection->rotationFromBone(), wGlobal);
            connection->_tGlobal = tGlobal + Math::rotate(connection->translationFromBone(), wGlobal);
        }
    }
}

std::set<SkeletonComponent*> SkeletonComponent::connectedComponents() const {
    if (const Bone* bone = dynamic_cast<const Bone*>(this)) {
        std::set<Connection*> connections = bone->connections();
        return std::set<SkeletonComponent*>(connections.begin(), connections.end());
    }
    else if (const Connection* connection = dynamic_cast<const Connection*>(this)) {
        return std::set<SkeletonComponent*>({ connection->opposingConnection(), connection->bone() });
    }
}

TreeNode<SkeletonComponent*>* SkeletonComponent::buildTreeToTargets(std::set<SkeletonComponent*> targets) {

    if (targets.size() == 0) return new TreeNode<SkeletonComponent*>();

    TreeNode<SkeletonComponent*>* root = new TreeNode<SkeletonComponent*>(this);

    std::vector<TreeNode<SkeletonComponent*>*> stack({ root });
    std::set<SkeletonComponent*> visited;

    std::vector<TreeNode<SkeletonComponent*>*> targetNodes;

    TreeNode<SkeletonComponent*>* tree;
    do {
        tree = stack.back();
        stack.pop_back();

        SkeletonComponent* data = tree->data();
        if (targets.find(data) != targets.end()) targetNodes.push_back(tree);

        visited.insert(data);

        for (auto component : data->connectedComponents()) {
            if (visited.find(component) == visited.end()) {
                TreeNode<SkeletonComponent*>* subTree = new TreeNode<SkeletonComponent*>(component, tree);
                stack.push_back(subTree);
            }
        }
    } while (stack.size() > 0 && targetNodes.size() < targets.size());

    root->pruneToLeafset(std::set<TreeNode<SkeletonComponent*>*>(targetNodes.begin(), targetNodes.end()));
    return root;
}