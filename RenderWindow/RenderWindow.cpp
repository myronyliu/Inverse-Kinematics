// RenderWindoeMain.cpp : Creates the 3D Viewer.

#include "stdafx.h"
#include "GlutUI.h"
#include "Paths.h"
#include "Skeleton.h"

Scene::Path* tipPath;
Scene::Path* anchorPath;

void idle(void) {
    //glutPostRedisplay();
}

GlutUI::Manager MANAGER;
int main(int argc, char* argv[])
{
    auto randRotation = []() {
        return 2 * M_PI*glm::vec3(rand(), rand(), rand()) / (float)RAND_MAX;
    };

    MANAGER.init(argc, argv);
    int windowWidth = 700;
    int windowHeight = 700;
    GlutUI::Window & mainWindow = MANAGER.createWindow(windowWidth, windowHeight, "Render Window");
    GlutUI::Panel & mainPanel = MANAGER.createPanel(mainWindow, windowWidth, windowHeight, "Render Panel");
    Scene::World world = Scene::createWorld();
    std::cout << std::string((char *)glGetString(GL_VENDOR)) << std::endl;
    std::cout << std::string((char *)glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL " << std::string((char *)glGetString(GL_VERSION)) << std::endl;
    std::cout << "====================================================" << std::endl;
    
    
    glutIdleFunc(idle);

    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHTING);
    world.addLight(glm::vec3(10, 10, 10), glm::vec4(1, 1, 1, 1));

    Scene::Arrow * xAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(1, 0, 0));
    Scene::Arrow * yAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(0, 1, 0));
    Scene::Arrow * zAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(0, 0, 1));
    xAxis->setColor(glm::vec4(1, 0, 0, 1));
    yAxis->setColor(glm::vec4(0, 1, 0, 1));
    zAxis->setColor(glm::vec4(0, 0, 1, 1));
    world.addObject(xAxis);
    world.addObject(yAxis);
    world.addObject(zAxis);

    tipPath = new Scene::Path(1.5);
    anchorPath = new Scene::Path(1);
    tipPath->setParameterization(PathParameterizations::circle);
    anchorPath->setParameterization(PathParameterizations::line);
    anchorPath->setTranslation(glm::vec3(0, 0, 0));
    world.addObject(tipPath);
    world.addObject(anchorPath);

    std::vector<Joint*> childrenJoints(3);
    childrenJoints[0] = new BallJoint(glm::vec3(-1, 0, 0), glm::vec3(0, -M_PI / 2, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    childrenJoints[1] = new BallJoint(glm::vec3(1, 0, 0), glm::vec3(0, M_PI / 2, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    childrenJoints[2] = new BallJoint(glm::vec3(0, -1, 0), glm::vec3(M_PI / 2, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    /*childrenJoints[3] = new BallJoint(glm::vec3(0, 1, 0), glm::vec3(-M_PI / 2, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    childrenJoints[4] = new BallJoint(glm::vec3(0, 0, -1), glm::vec3(M_PI, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    childrenJoints[5] = new BallJoint(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));*/

    std::vector<Bone*> childrenBones(6);
    for (int i = 0; i < 3; i++) {
        childrenBones[i] = new Bone();
        childrenJoints[i]->couple(childrenBones[i]);
    }

    Bone* root = new Bone(childrenJoints);

    /*Bone* bone0 = new Bone();
    Bone* bone1 = new Bone();
    Bone* bone2 = new Bone();

    Joint* joint01 = new BallJoint(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    Joint* joint12 = new BallJoint(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 0, 0));*/
    
    /*bone0->attach(joint01)->couple(bone1)->attach(joint12)->couple(bone2);
    bone1->attach(joint01)->couple(bone0);
    bone1->attach(joint12)->couple(bone2);*/

    Scene::Skeleton* skeleton = new Scene::Skeleton();
    skeleton->setRoot(root);
    //skeleton->setRotation(glm::vec3(M_PI / 4, 0, 0));
    //skeleton->setTranslation(glm::vec3(0.1, 0.2, 0.3));
    world.addObject(skeleton);


    Scene::Camera * cam = new Scene::Camera();
    cam->setPos(glm::vec3(8, 0, 0));
    cam->setDir(glm::vec3(-1, 0, 0));
    mainPanel.setWorld(&world);
    mainPanel.setCamera(cam);
    GlutUI::Controls::Keyboard keyboard(&mainPanel, mainPanel.getCamera());
    GlutUI::Controls::Mouse mouse(&mainPanel, mainPanel.getCamera());

    ///////////////////////////////////////
    ///// Keyboard Hotkey Assignments /////
    ///////////////////////////////////////
    auto ilambda = [&]() {
        //skeleton->jiggle();
    };
    auto jlambda = [&]() {
    };
    auto nlambda = [&]() {
        
    };
    keyboard.register_hotkey('i', ilambda);
    keyboard.register_hotkey('j', jlambda);
    keyboard.register_hotkey('n', nlambda);


    MANAGER.drawElements();

    return 0;
}


