#include "BodyComponents.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;

void Scene::updateGlobals(TreeNode<SkeletonComponent*>* componentTree) {
    std::vector<TreeNode<SkeletonComponent*>*> seqn = componentTree->DFSsequence();

    SkeletonComponent* root = seqn[0]->data();
    TransformStack transformStack(root->globalTranslation(), root->globalRotation());

    for (int i = 1; i < seqn.size(); i++) {
        SkeletonComponent* component = seqn[i]->data();

        int depth = seqn[i]->depth();
        int previousDepth = seqn[i - 1]->depth();

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

void Scene::updateGlobals(const std::vector<SkeletonComponent*>& components) {

    SkeletonComponent* root = components[0];
    TransformStack transformStack(root->globalTranslation(), root->globalRotation());

    for (int i = 1; i < components.size(); i++) {
        SkeletonComponent* component = components[i];

        glm::vec3 t, w;
        SkeletonComponent* previousComponent = components[i - 1];
        auto transformsToConnectedComponents = previousComponent->transformsToConnectedComponents();
        auto it = transformsToConnectedComponents.find(component);

        if (it == transformsToConnectedComponents.end()) return;

        std::tie(t, w) = it->second;

        transformStack.push();
        transformStack.translate(t);
        transformStack.rotate(w);

        component->setGlobalTranslation(transformStack.getTranslation());
        component->setGlobalRotation(transformStack.getRotation());
    }
}


void Scene::linearIK(const std::vector<SkeletonComponent*>& armBaseToTip, const glm::vec3& tipTarget) {

    SkeletonComponent* tip = armBaseToTip.back();

    auto restoreAll = [&]() {
        for (auto component : armBaseToTip)
            component->restore();
    };
    std::vector<Socket*> sockets;
    for (auto component : armBaseToTip) {
        component->backup();
        if (Socket* socket = dynamic_cast<Socket*>(component))
            sockets.push_back(socket);
    }

    glm::vec3 tipPosition = tip->globalTranslation();
    glm::vec3 stepToTarget = tipTarget - tipPosition;
    float distanceToTarget = glm::length(stepToTarget);

    while (distanceToTarget>0.0001f) {

        for (auto socket : sockets) {
            socket->nudge(tip, stepToTarget);
        }
        Scene::updateGlobals(armBaseToTip);

        glm::vec3 newTipPosition = tip->globalTranslation();
        glm::vec3 newStepToTarget = tipTarget - newTipPosition;
        float newDistanceToTarget = glm::length(newStepToTarget);

        if (newDistanceToTarget < distanceToTarget) {
            distanceToTarget = newDistanceToTarget;
            tipPosition = newTipPosition;
            stepToTarget = newStepToTarget;
        }
        else {
            restoreAll();
            stepToTarget /= 2;
        }
    }
}