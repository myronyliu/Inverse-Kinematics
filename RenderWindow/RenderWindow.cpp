// RenderWindoeMain.cpp : Creates the 3D Viewer.

#include "stdafx.h"
#include "GlutUI.h"
#include "Paths.h"
#include "Body.h"
#include "MakeSkeleton.h"

Scene::Path* tipPath;
Scene::Path* anchorPath;
Scene::Body* body;
Scene::Bone* bone;

void idle(void) {
    /*body->setTranslation(bone, tipPath->stepT(0.01));
    glutPostRedisplay();*/
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

    Scene::Arrow * xAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(2, 0, 0));
    Scene::Arrow * yAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(0, 2, 0));
    Scene::Arrow * zAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(0, 0, 2));
    xAxis->setColor(glm::vec4(1, 0, 0, 1));
    yAxis->setColor(glm::vec4(0, 1, 0, 1));
    zAxis->setColor(glm::vec4(0, 0, 1, 1));
    world.addObject(xAxis);
    world.addObject(yAxis);
    world.addObject(zAxis);

    tipPath = new Scene::Path(4.0);
    anchorPath = new Scene::Path(1);
    tipPath->setParameterization(PathParameterizations::circle);
    anchorPath->setParameterization(PathParameterizations::line);
    anchorPath->setTranslation(glm::vec3(0, 0, 0));
    world.addObject(tipPath);
    world.addObject(anchorPath);

    std::tie(body, bone) = test(2);
    world.addObject(body);


    Scene::Camera * cam = new Scene::Camera();
    cam->setPos(16.0f*glm::vec3(1, 1, 1));
    cam->setDir(-cam->pos());
    cam->setUp(glm::vec3(0, 0, 1));
    mainPanel.setWorld(&world);
    mainPanel.setCamera(cam);
    GlutUI::Controls::Keyboard keyboard(&mainPanel, mainPanel.getCamera());
    GlutUI::Controls::Mouse mouse(&mainPanel, mainPanel.getCamera());

    ///////////////////////////////////////
    ///// Keyboard Hotkey Assignments /////
    ///////////////////////////////////////
    auto ilambda = [&]() {
        //body->jiggle(2);
        body->setTranslation(bone, glm::vec3(0, 1, 0));
        
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


