// RenderWindoeMain.cpp : Creates the 3D Viewer.

#include "stdafx.h"
#include "GlutUI.h"
#include "Paths.h"
#include "Skeleton.h"

Scene::Path* tipPath;
Scene::Path* anchorPath;

void idle(void) {
    
    glutPostRedisplay();
}

GlutUI::Manager MANAGER;
int main(int argc, char* argv[])
{
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
    world.addLight(glm::vec3(0, 10, 0), glm::vec4(1, 1, 1, 1));

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


    Bone* root = new Bone();
    Bone* neighbor = new Bone();
    HalfBallJoint* hj0 = new HalfBallJoint(glm::vec3(0, 0, 0.5), glm::vec3(0, 0, 0));
    HalfBallJoint* hj1 = new HalfBallJoint(glm::vec3(0, 0, -0.5), glm::vec3(M_PI/4, 0, 0));

    root->attach(hj0);
    neighbor->attach(hj1);

    hj0->setOpposingHalfJoint(hj1);
    
    
    Scene::Skeleton* skeleton = new Scene::Skeleton();
    skeleton->setRoot(root);
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
    auto ilambda = []() {
        std::string bmpName = "out.bmp";
        SaveAsBMP(bmpName.c_str());
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


