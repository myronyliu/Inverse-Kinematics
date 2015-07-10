// RenderWindoeMain.cpp : Creates the 3D Viewer.

#include "stdafx.h"
#include "GlutUI.h"
#include "Paths.h"

Scene::Arm* arm;

void idle(void) {
    //arm->nudgeTip(0.001f*glm::vec3(0, 1, 1));
    //arm->jiggle(M_PI / 1024, M_PI / 1024);
    glutPostRedisplay();
}

GlutUI::Manager MANAGER;
int main(int argc, char* argv[])
{
    MANAGER.init(argc, argv);
    int windowWidth = 512;
    int windowHeight = 512;
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

    Scene::Path * linePath = new Scene::Path;
    linePath->setParameterization(PathParameterizations::circle);
    world.addObject(linePath);

    arm = new Scene::Arm(std::vector<float>({ 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f }));
    arm = new Scene::Arm(std::vector<float>({ 1.0f }));
    for (int i = 0; i < arm->nJoints(); i++) {
        arm->setLocalRotation(i, 2 * M_PI*glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX));
    }
    //arm->setRotation(glm::vec3(0.25*M_PI, 0, 0));
    ///arm->setLocalRotationAxis(0, glm::vec3(0, 1, 0));
    //arm->setLocalRotation(0, glm::vec3(0, 0.25*M_PI, 0));
    //arm->setTranslation(glm::vec3(1, 1, 1));

    Scene::Sphere* sphere = new Scene::Sphere(glm::vec3(0,0,-0.566),0.05);
    //world.addObject(sphere);

    //arm = new Scene::Arm(std::vector<float>({ 1.0f, 1.0f }));
    //arm->setLocalRotation(0,glm::vec3(0.1, 0.2, 0.3));

    world.addObject(arm);

    Scene::Camera * cam = new Scene::Camera();
    cam->setPos(glm::vec3(10, 0, 0));
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
        //arm->jiggle(M_PI / 256, M_PI / 256);
        /*for (int i = 0; i < arm->nJoints(); i++) {
            arm->setLocalRotation(i, 2 * M_PI*glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX));
        }*/
        //arm->setLocalRotationAngle(0,arm->localRotationAngle(0)+0.01);
    };
    auto nlambda = [&]() {
        arm->nudgeTip(0.01f*glm::vec3(0, 1, 1));
    };
    keyboard.register_hotkey('i', ilambda);
    keyboard.register_hotkey('j', jlambda);
    keyboard.register_hotkey('n', nlambda);


    MANAGER.drawElements();

    return 0;
}


