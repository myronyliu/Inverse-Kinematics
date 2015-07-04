// RenderWindoeMain.cpp : Creates the 3D Viewer.

#include "stdafx.h"
#include "GlutUI.h"
#include "Paths.h"


void SaveAsBMP(const char *fileName)
{
    FILE *file;
    unsigned long imageSize;
    GLbyte *data = NULL;
    GLint viewPort[4];
    GLenum lastBuffer;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    bmfh.bfType = 'MB';
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = 54;
    glGetIntegerv(GL_VIEWPORT, viewPort);
    imageSize = ((viewPort[2] + ((4 - (viewPort[2] % 4)) % 4))*viewPort[3] * 3) + 2;
    bmfh.bfSize = imageSize + sizeof(bmfh) + sizeof(bmih);
    data = (GLbyte*)malloc(imageSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_SWAP_BYTES, 1);
    glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, viewPort[2], viewPort[3], GL_BGR, GL_UNSIGNED_BYTE, data);
    data[imageSize - 1] = 0;
    data[imageSize - 2] = 0;
    glReadBuffer(lastBuffer);
    std::FILE* f;
    fopen_s(&f, fileName, "wb");
    file = f;
    bmih.biSize = 40;
    bmih.biWidth = viewPort[2];
    bmih.biHeight = viewPort[3];
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;
    bmih.biCompression = 0;
    bmih.biSizeImage = imageSize;
    bmih.biXPelsPerMeter = 45089;
    bmih.biYPelsPerMeter = 45089;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;
    fwrite(&bmfh, sizeof(bmfh), 1, file);
    fwrite(&bmih, sizeof(bmih), 1, file);
    fwrite(data, imageSize, 1, file);
    free(data);
    fclose(file);
}//
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

    Scene::Shader* rainbowShader = new Scene::Shader("shaders/rainbow_vert.glsl", "shaders/rainbow_frag.glsl");
/*
    Scene::Arrow * xAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(2, 0, 0));
    Scene::Arrow * yAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(0, 2, 0));
    Scene::Arrow * zAxis = new Scene::Arrow(glm::vec3(-0, -0, -0), glm::vec3(0, 0, 2));
    xAxis->setColor(glm::vec4(1, 0, 0, 1));
    yAxis->setColor(glm::vec4(0, 1, 0, 1));
    zAxis->setColor(glm::vec4(0, 0, 1, 1));
    world.addObject(xAxis);
    world.addObject(yAxis);
    world.addObject(zAxis);*/

    Scene::Cylinder* xCylinder = new Scene::Cylinder(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.1);
    Scene::Cylinder* yCylinder = new Scene::Cylinder(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 0.1);
    Scene::Cylinder* zCylinder = new Scene::Cylinder(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 0.1);
    xCylinder->setColor(glm::vec4(1, 0, 0, 1));
    yCylinder->setColor(glm::vec4(0, 1, 0, 1));
    zCylinder->setColor(glm::vec4(0, 0, 1, 1));
    world.addObject(xCylinder);
    world.addObject(yCylinder);
    world.addObject(zCylinder);

    Scene::Path * linePath = new Scene::Path;
    linePath->setParameterization(PathParameterizations::circle);
    world.addObject(linePath);



    Scene::Camera * cam = new Scene::Camera();
    cam->setPos(glm::vec3(0, 0, 4));
    cam->setDir(glm::vec3(0, 0, -1));
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
    keyboard.register_hotkey('i', ilambda);

    MANAGER.drawElements();

    return 0;
}
