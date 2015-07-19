#ifndef _BODYCOMPONENTS_H_
#define _BODYCOMPONENTS_H_

#include "stdafx.h"
#include "Rotation.h"
#include "GlutDraw.h"
#include "utils.h"
#include "Math.h"

enum {
    PIN = 0,
    BALL = 1,
    PRISM = 2
};

namespace Scene {

    class Bone;
    class Joint;
    class Socket;
    class Connection;
    class Skeleton;

    class SkeletonComponent // Wrapper class for Bones and Connections (Sockets and Joints)
    {
    public:
        glm::vec3 _globalTranslation;
        glm::vec3 _globalRotation;
    };

    

    class Bone : SkeletonComponent
    {
        friend class Joint;
        friend class Socket;
        friend class Skeleton;
        friend class Connection;
    public:
        Bone() : _sockets(std::set<Socket*>()), _joints(std::set<Joint*>()) {}
        Bone(std::vector<Socket*> sockets, std::vector<Joint*> joints);
        void draw(const float& scale = 1) const;
        virtual void doDraw(const float& scale = 0.2) const;

        Joint* attach(Joint*);
        std::vector<Joint*> attach(std::vector<Joint*> joints) { for (auto joint : joints) attach(joint); return joints; }
        void detach(Joint*);

        Socket* attach(Socket*);
        std::vector<Socket*> attach(std::vector<Socket*> sockets) { for (auto socket : sockets) attach(socket); return sockets; }
        void detach(Socket*);

        std::map<Bone*, std::vector<Connection*>> Bone::reachableBonePaths(Connection* = NULL);
        std::set<Bone*> reachableBones(Connection* = NULL);
        Skeleton* addToSkeleton(Skeleton*, Connection* = NULL);

        Skeleton* skeleton() const { return _skeleton; }
        std::set<Joint*> joints() const { return _joints; }
        std::set<Socket*> sockets() const { return _sockets; }
        std::set<Connection*> connections() const;

        std::map<Connection*, Bone*> connectionToBones() const;
        std::map<Socket*, Bone*> socketToBones() const;
        std::map<Joint*, Bone*> jointToBones() const;

        Connection* getConnectionToBone(Bone*) const;

    protected:
        Skeleton* _skeleton;
        std::set<Joint*> _joints;
        std::set<Socket*> _sockets;
    private:
        bool insertConnection(Connection*);
        bool eraseConnection(Connection*);
    };



    class Connection : public SkeletonComponent
    {
        friend class Bone;
        friend class Skeleton;
    public:
        Connection(const int& = 4, const float& = 1, Bone* = NULL);
        Connection(Bone* bone, const glm::vec3& t, const glm::vec3& w) :
            _bone(bone), _translationFromBone(t), _rotationFromBone(AxisAngleRotation2(w)) {}

        virtual void draw(const float&) const;
        virtual void drawAnchor(const float&) const {
            GlutDraw::drawCylinder(_translationFromBone / 2.0f, _translationFromBone / 2.0f, 0.02);
        }
        virtual void drawPivot(const float&) const = 0;

        // The following is expressed in the frame of _bone
        virtual std::pair<glm::vec3, AxisAngleRotation2> alignAnchorToTarget() = 0;
        virtual std::pair<glm::vec3, AxisAngleRotation2> alignThisToBone() = 0;

        /////////////////
        //// GETTERS ////
        /////////////////

        Skeleton* skeleton();
        Connection* opposingConnection();
        Bone* opposingBone();
        std::pair<Socket*, Joint*> socketJoint();
        glm::vec3 translationToOpposingConnection();
        AxisAngleRotation2 rotationToOpposingConnection();
        std::pair<glm::vec3, AxisAngleRotation2> transformsToConnection(const std::vector<Connection*>&);

        Bone* bone() const { return _bone; }
        glm::vec3 translationFromBone() const { return _translationFromBone; }
        AxisAngleRotation2 rotationFromBone() const { return _rotationFromBone; }
        AxisAngleRotation2 rotationFromBone2() const { return _rotationFromBone; }
        glm::vec3 rotationFromBone3() const { return _rotationFromBone.axisAngleRotation3(); }
        glm::mat3 rotationFromBoneR() const { return _rotationFromBone.rotationMatrix(); }

        /////////////////
        //// SETTERS ////
        /////////////////

        void setTranslationFromBone(const glm::vec3& translation) { _translationFromBone = translation; }
        void setRotationFromBone(const AxisAngleRotation2& rotation) { _rotationFromBone = rotation; _rotationFromBone.clamp(); }
        void setRotationFromBone(const glm::vec3& w) { _rotationFromBone = AxisAngleRotation2(w); _rotationFromBone.clamp(); }
        void setRotationFromBone(const glm::mat3& R) { _rotationFromBone = AxisAngleRotation2(R); _rotationFromBone.clamp(); }

        Bone* attach(Bone* bone);
        void dettach();

    protected:
        Bone* _bone;
        glm::vec3 _translationFromBone;
        AxisAngleRotation2 _rotationFromBone;
        
    };

    class Joint : public Connection
    {
        friend class Socket;
        friend class Skeleton;
    public:
        Joint(const int& i = 4, const float& scale = 1, Bone* bone = NULL) : Connection(i, scale, NULL) { Connection::attach(bone); }
        Joint(Bone* bone, const glm::vec3& t, const glm::vec3& w) : Connection(bone, t, w) {}

        Socket* couple(Socket* socket);
        void decouple();

        Socket* socket() const { return _socket; }

        std::pair<glm::vec3, AxisAngleRotation2> alignAnchorToTarget();
        std::pair<glm::vec3, AxisAngleRotation2> alignThisToBone();

        virtual int type() const { return -1; }
    protected:

        Socket* _socket;
    };

    class Socket :public Connection
    {
        friend class Joint;
    public:
        Socket(const int& i = 4, const float& scale = 1, Bone* bone = NULL);
        Socket(Bone* bone, const glm::vec3& t, const glm::vec3& w) :
            Connection(bone, t, w), _translationToJoint(glm::vec3(0, 0, 0)), _rotationToJoint(AxisAngleRotation2(glm::vec2(0, 0), 0)) {}

        /////////////////
        //// DRAWING ////
        /////////////////

        Joint* couple(Joint*);
        void decouple();

        //////////////////////////
        //// PARAMETERIZATION ////
        //////////////////////////

        virtual void constrainParams() {}
        virtual std::map<int, float> adjustableParams() const { return _params; }
        void setParams(const std::map<int, float>& params_unconstrained);
        void setParam(const int& key, const float& value);
        void setConstraint(const int& key, const float& value);

        void restore() { _params = _stashedParams; }
        void backup() { _stashedParams = _params; }

        void perturbJoint(const float& scale = 1) { perturbParams(scale); constrainParams(); buildTransformsFromParams(); }
        virtual float reach() const { return 0; }

        /////////////////
        //// GETTERS ////
        /////////////////

        std::map<int, float> params() const { return _params; }
        bool getConstraint(const int& key, float& value) const;
        bool getParam(const int& key, float& value) const;

        glm::vec3 translationToJoint() const { return _translationToJoint; }
        AxisAngleRotation2 rotationToJoint() const { return _rotationToJoint; }
        AxisAngleRotation2 rotationToJoint2() const { return _rotationToJoint; }
        glm::vec3 rotationToJoint3() const { return _rotationToJoint.axisAngleRotation3(); }
        glm::mat3 rotationToJointR() const { return _rotationToJoint.rotationMatrix(); }
        Joint* joint() const { return _joint; }

        std::pair<glm::vec3, AxisAngleRotation2> alignAnchorToTarget();
        std::pair<glm::vec3, AxisAngleRotation2> alignThisToBone();

        // in the name of the function below, Params refers to the params of the socket
        // Tip refers to the translation from the local coordinate system origin to the position of the argument
        virtual arma::mat Jacobian_ParamsToTip(Connection*);

        virtual int type() const { return -2; }
    protected:
        //////////////////////////////////////////////////////////////////
        //// DANGEROUS FUNCTIONS THAT SHOULD NOT BE ACCESSED PUBLICLY ////
        //////////////////////////////////////////////////////////////////

        virtual void perturbParams(const float& scale) {}
        virtual void buildTransformsFromParams() {}
        virtual void buildParamsFromTransforms() {}


        /////////////////////////////////////////////////////////////////////////////////////////////
        //// The following should always be called via the connection, so we make them protected ////
        /////////////////////////////////////////////////////////////////////////////////////////////

        void setTranslationToJoint(const glm::vec3& translation);
        void setRotationToJoint(const AxisAngleRotation2& rotation);
        void setRotationToJoint(const glm::vec3& w);
        void setRotationToJoint(const glm::mat3& R);

        //////////////////////////
        //// MEMBER VARIABLES ////
        //////////////////////////

        Joint* _joint;

        glm::vec3 _translationToJoint;            // IN THE COORDINATE FRAME OF THE JOINT's "BASE" (in "default" configuration)
        AxisAngleRotation2 _rotationToJoint;

        std::map<int, float> _constraints;  // Constraints are keyed on indices of our choosing
        std::map<int, float> _params;
        std::map<int, float> _stashedParams;
    };

    class Skeleton
    {
        friend class Bone;
    public:
        Skeleton() : _bones(std::set<Bone*>()) {}
        Skeleton(Bone* bone) { _bones = bone->reachableBones(); for (auto bone : _bones) bone->_skeleton = this; }

        std::set<std::pair<Socket*, Joint*>> socketJoints() const;
        std::set<Bone*> bones() const { return _bones; }
        std::set<Socket*> sockets() const;
        std::set<Joint*> joints() const;

        void jiggle(const float& amplitude = 1) { for (auto socket : sockets()) socket->perturbJoint(amplitude); }
    private:
        std::set<Bone*> _bones;
    };

}
#endif