#include "MakeSkeleton.h"
#include "BallSocketJoint.h"

using namespace Scene;

Body* axisTree(const int& depth) {

    auto randRot = []() {
        float x = 2.0f*(float)rand() / RAND_MAX - 1.0f;
        float y = 2.0f*(float)rand() / RAND_MAX - 1.0f;
        float z = 2.0f*(float)rand() / RAND_MAX - 1.0f;
        return glm::vec3(x,y,z);
    };

    std::vector<Socket*> rootSockets(6);
    for (int i = 0; i < rootSockets.size(); i++) {
        rootSockets[i] = new BallSocket(i);
        //rootSockets[i]->setTranslationFromBone(randRot());
        /*rootSockets[i]->setConstraint(0, 0.4*M_PI);
        rootSockets[i]->setConstraint(1, 0.6*M_PI);
        rootSockets[i]->setConstraint(2, 0);
        rootSockets[i]->setConstraint(3, M_PI / 4);*/
    }

    Bone* root = new Bone(rootSockets, std::vector<Joint*>());

    for (auto rootSocket : rootSockets) {
        std::vector<Socket*> sockets(5);
        for (int i = 0; i < sockets.size(); i++) {
            sockets[i] = new BallSocket(i, 0.5);
            //rootSockets[i]->setRotationFromBone(randRot());
            /*rootSocket->setConstraint(0, 0.4*M_PI);
            rootSocket->setConstraint(1, 0.6*M_PI);*/
        }
        for (auto socket : sockets) {
            socket->couple(new BallJoint(0, 0.5, new Bone()));
        }

        rootSocket->couple(new BallJoint(5))->attach(new Bone())->attach(sockets);

    }

    Scene::Skeleton* skeleton = new Scene::Skeleton(root);
    return new Scene::Body(skeleton);
}

Body* chain(const int& nJoints) {
    Bone* root = new Bone();
    Bone* bone = root;
    for (int i = 0; i < nJoints; i++) {
        Bone* nextBone = new Bone();
        bone->attach(new BallSocket())->couple(new BallJoint(5))->attach(nextBone);
        bone = nextBone;
    }
    
    Scene::Skeleton* skeleton = new Scene::Skeleton(root);
    return new Scene::Body(skeleton);
}