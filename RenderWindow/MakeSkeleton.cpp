#include "MakeSkeleton.h"
#include "BallSocketJoint.h"

using namespace Scene;

Body* axisTree(const int& maxDepth) {

    int nBranches = 5;

    std::vector<std::pair<Bone*, int>> stack({ std::make_pair(new Bone(), 0) });

    Bone *root = NULL;

    Bone *bone = NULL;
    int depth;
    do {
        std::tie(bone, depth) = stack.back();
        stack.pop_back();

        if (depth == maxDepth) continue;

        std::vector<Socket*> sockets(nBranches);
        for (int i = 0; i < nBranches; i++) {
            Bone *newBone = new Bone();
            sockets[i] = new BallSocket(i, pow(0.5f, depth));
            sockets[i]->couple(new BallJoint(5, pow(0.5, depth), newBone));
            stack.push_back(std::make_pair(newBone, depth + 1));

            //sockets[i]->setConstraint(1, 0.0*M_PI);
            //sockets[i]->setConstraint(4, 0.0*M_PI);
            //sockets[i]->setConstraint(5, 0.0*M_PI);
        }
        bone->attach(sockets);
        if (depth == 0) root = bone;
    } while (stack.size() > 0);

    Scene::Skeleton *skeleton = new Scene::Skeleton(bone);
    Scene::Body *body = new Scene::Body(skeleton);
    body->anchor(root);
    body->hardUpdate();
    return body;
}

std::pair<Body*, Bone*> chain(const int& nJoints) {
    Bone* root = new Bone();
    Bone* bone = root;
    Bone* asdf = NULL;
    for (int i = 0; i < nJoints; i++) {
        Bone* nextBone = new Bone();
        if (i == 0) asdf = nextBone;
        auto asdf = new BallSocket();
        //asdf->setConstraint(1, 0.3f*M_PI);
        bone->attach(asdf)->couple(new BallJoint(5))->attach(nextBone);
        bone = nextBone;
    }

    Scene::Skeleton* skeleton = new Scene::Skeleton(root);
    Scene::Body* body = new Scene::Body(skeleton);
    body->anchor(root, true, true);
    body->hardUpdate();
    return std::make_pair(body, bone);
    //return body;
}


std::pair<Body*, Bone*> starfish(const int& nLegs, const int& nJoints) {

    Bone* hubBone = new Bone();

    std::vector<Socket*> hubSockets(nLegs,NULL);
    for (int i = 0; i < hubSockets.size(); i++) {
        float phi = (2 * M_PI / hubSockets.size())*i;
        glm::vec3 t = glm::vec3(cos(phi), sin(phi), 0);
        glm::vec3 w = Math::axisAngleAlignZtoVEC3(t);
        hubSockets[i] = new BallSocket(4);
        hubSockets[i]->couple(new BallJoint(NULL,t,w))->attach(hubBone);
    }

    std::vector<Bone*> leaves(nLegs,NULL);
    for (int i = 0; i < leaves.size();i++) {
        leaves[i] = new Bone();
        Bone* bone = leaves[i];
        for (int i = 0; i < nJoints; i++) {
            Bone* nextBone = new Bone();
            bone->attach(new BallSocket(4))->couple(new BallJoint(5))->attach(nextBone);
            bone = nextBone;
        }
        bone->attach(hubSockets[i]);
    }

    Scene::Skeleton* skeleton = new Scene::Skeleton(hubBone);
    Scene::Body* body = new Scene::Body(skeleton);
    body->anchor(hubBone, true, true);
    body->hardUpdate();
    return std::make_pair(body, leaves[0]);
    //return body;
}