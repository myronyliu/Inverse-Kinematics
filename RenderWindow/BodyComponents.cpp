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

    glm::vec3 t, w;

    auto warn = [&]() {
        auto reupdate = [&]() {
            transformStack.pop();
            transformStack.push();
            transformStack.translate(t);
            transformStack.rotate(w);
        };
        glm::vec3 trans = transformStack.getTranslation();
        glm::vec3 rot = transformStack.getRotation();
        if (!isfinite(trans[0]) || !isfinite(trans[1]) || !isfinite(trans[2])) {
            reupdate();
        }
        if (!isfinite(rot[0]) || !isfinite(rot[1]) || !isfinite(rot[2])) {
            reupdate();
        }
    };

    warn();

    for (int i = 1; i < components.size(); i++) {
        SkeletonComponent* component = components[i];
        SkeletonComponent* previousComponent = components[i - 1];
        auto transformsToConnectedComponents = previousComponent->transformsToConnectedComponents();
        auto it = transformsToConnectedComponents.find(component);

        if (it == transformsToConnectedComponents.end()) return;

        std::tie(t, w) = it->second;

        transformStack.translate(t);
        transformStack.rotate(w);

        warn();

        component->setGlobalTranslation(transformStack.getTranslation());
        component->setGlobalRotation(transformStack.getRotation());
    }
}


void Scene::linearIK(const std::vector<SkeletonComponent*>& armBaseToTip, const glm::vec3& tipTarget) {

    SkeletonComponent* tip = armBaseToTip.back();

    auto backupAll = [&]() {
        for (auto component : armBaseToTip)
            component->backup();
    };

    auto restoreAll = [&]() {
        for (auto component : armBaseToTip)
            component->restore();
    };

    backupAll();

    std::vector<Connection*> forwardConnections;
    for (int i = 0; i < armBaseToTip.size() - 1; i++) {
        SkeletonComponent* component = armBaseToTip[i];
        if (Connection* forwardConnection = dynamic_cast<Connection*>(component)) {
            if (forwardConnection->opposingConnection() != NULL) {
                forwardConnections.push_back(forwardConnection);
                i += 2;
                continue;
            }
        }
    }

    glm::vec3 tipPosition = tip->globalTranslation();
    glm::vec3 stepToTarget = tipTarget - tipPosition;
    float distanceToTarget = glm::length(stepToTarget);

    bool success = false;
    int maxTries = 64;
    int tries = 0;
    while (distanceToTarget > 0.01f && tries < maxTries) {

        for (auto forwardConnection : forwardConnections) {
            forwardConnection->nudge(tip, stepToTarget, DOWNSTREAM);
        }
        Scene::updateGlobals(armBaseToTip);

        glm::vec3 newTipPosition = tip->globalTranslation();
        glm::vec3 newStepToTarget = tipTarget - newTipPosition;
        float newDistanceToTarget = glm::length(newStepToTarget);

        if (newDistanceToTarget < distanceToTarget) {
            backupAll();
            distanceToTarget = newDistanceToTarget;
            tipPosition = newTipPosition;
            stepToTarget = newStepToTarget;
            tries = 0;
            success = true;
        }
        else {
            restoreAll();
            stepToTarget /= 2;
            tries++;
        }
    }
    if (!success) for (auto forwardConnection : forwardConnections) {
        forwardConnection->perturbCoupling();
    }
}
void Scene::linearIK(const glm::vec3& tipTarget, const std::vector<SkeletonComponent*>& armTipToBase) {
    int n = armTipToBase.size();
    std::vector<SkeletonComponent*> armBaseToTip(n, NULL);
    for (int i = 0; i < n; i++) {
        armBaseToTip[i] = armTipToBase[n - 1 - i];
    }
    linearIK(armBaseToTip, tipTarget);
}