/** Display.cpp
 * 
 *
 * 
**/

#include "GlutUI.h"

using namespace GlutUI;
/** Global variables **/
int UIElement::NEXTID = 0;
//TODO Hacks. Make sure these are actually the right window and controls.
Window * mainWindow;
Controls::Mouse * mainMouse;
Controls::Keyboard * mainKeyboard;

//char * textFileRead(const char * fn);

/** Method Definitions **/

/** Manager Class **/
/** Manages and exposes all the UI elements
 *
**/

void Manager::init(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
}

void Manager::drawElements()
{
    glutMainLoop();
}

//void Manager::setShaders(const std::string vertfile, const std::string fragfile)
//{
//    GLint GlewInitResult = glewInit();
//    if (GLEW_OK != GlewInitResult)
//    {
//        printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
//        exit(EXIT_FAILURE);
//    }

//    GLuint v, f, p;
//    char *vs,*fs;

//    if (vertfile == "" && fragfile == ""){ return;  }
//    p = glCreateProgram();

//    if (vertfile != "")
//    {
//        v = glCreateShader(GL_VERTEX_SHADER);
//        vs = textFileRead(vertfile.c_str());
//        const char * vv = vs;
//        glShaderSource(v, 1, &vv,NULL);
//        free(vs);
//        glCompileShader(v);
//        glAttachShader(p,v);
//    }

//    if (fragfile != "")
//    {
//        f = glCreateShader(GL_FRAGMENT_SHADER);
//        fs = textFileRead(fragfile.c_str());
//        const char * ff = fs;
//        glShaderSource(f, 1, &ff, NULL);
//        free(fs);
//        glCompileShader(f);
//        glAttachShader(p, f);
//    }

//    glLinkProgram(p);
//    glUseProgram(p);
//}

//void Manager::deleteShaders()
//{
//    glDeleteShader(v);
//    glDeleteShader(f);
//}

Window & Manager::createWindow(int width, int height)
{
    Window * new_window = new Window(width, height);
    _windows.push_back(new_window);
    return *new_window;
}

Window & Manager::createWindow(int width, int height, std::string name)
{
    Window * new_window = new Window(width, height, name);
    _windows.push_back(new_window);
    return *new_window;
}

Panel & Manager::createPanel(Window & window, int width, int height, std::string name)
{
    Panel * new_panel = new Panel(width, height, name);
    window.addChildren(new_panel);
    _elements.push_back(new_panel);

    return *new_panel;
}

Panel & Manager::createPanel(Window & window, int width, int height, int xpos, int ypos, std::string name)
{
    Panel * new_panel = new Panel(width, height, xpos, ypos, name);
    window.addChildren(new_panel);
    _elements.push_back(new_panel);

    return *new_panel;
}

Button & Manager::createButton(Panel & panel, int width, int height, int xpos, int ypos, std::string name)
{
    Button * new_button = new Button(width, height, xpos, ypos, name);
    panel.addChildren(new_button);
    _elements.push_back(new_button);

    return *new_button;
}

/** UIElement Class **/
/** Abstract class that defines the basic properties of an UI element
 *
**/

void UIElement::init()
{
     _UIElemId = nextId();
}

/** Window Class **/
/** Abstract class that defines the basic properties of an UI element
 *
**/

void Window::init()
{
    _init = false;
    mainWindow = this;
    glutInitWindowSize(getWidth(), getHeight());
    glutInitWindowPosition(getXPos(), getYPos());
    glutCreateWindow(getName().c_str());
    glutDisplayFunc(Window::displayFuncWrapper);
    glutReshapeFunc(Window::reshapeFuncWrapper);
    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult)
    {
        std::cout << "ERROR: " << glewGetErrorString(GlewInitResult) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Window::displayFuncWrapper()
{
    mainWindow->draw();
}

void Window::reshapeFuncWrapper(int w, int h)
{
    mainWindow->reshape(w, h);
}

void Window::draw()
{    
    //std::cout << "Creating Window: " << getWidth() << " by " << getHeight() << "." << std::endl;

    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    //glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    for(std::vector<UIElement *>::const_iterator child = getChildren().begin() ;
        child < getChildren().end() ; child++)
    {
        (*child)->draw();
    }
    
    glutSwapBuffers();
}

void Window::reshape(int w, int h)
{
    //std::cout << "New size: " << w << " by " << h << std::endl;
    // prevent divide by 0 error when minimized
    if(w==0) 
        h = 1;

    setWidth(w);
    setHeight(h);
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(float)w/h,0.1,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


/** Panel Class **/
/** Panel class that defines a panel areas within the 
 *
**/

void Panel::draw()
{
    float asdf[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    if(_world != NULL)
    {
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, (float)getWidth() / getHeight(), 0.01, 1000000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMultMatrixf(_camera->rotMatData());
        //glMultMatrixf(asdf);
        glTranslatef(-_camera->pos()[0], -_camera->pos()[1], -_camera->pos()[2]);
        _world->draw();

        glPopMatrix();
    }
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, getWidth(), getHeight(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    for(std::vector<UIElement *>::const_iterator child = getChildren().begin() ;
        child < getChildren().end() ; child++)
    {
        (*child)->draw();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW); 
}

/** Button Class **/
/** Abstract class that defines the basic properties of an UI element
 *
**/

void Button::draw()
{
    glPushMatrix();
    glColor4d(1.0, 1.0, 1.0, 0.2);
    GlutDraw::drawRectangle(getXPos(), getYPos(), getWidth(), getHeight());
    
    glColor4d(1.0, 1.0, 1.0, 0.2);
    glBegin(GL_QUADS);
        glVertex2d(getXPos(), getYPos());
        glVertex2d(getXPos() + getWidth(), getYPos());
        glVertex2d(getXPos() + getWidth(), getYPos() + getHeight());
        glVertex2d(getXPos(), getYPos() + getHeight());
    glEnd();
}

void Controls::Mouse::init()
{
    mainMouse = this;
    glutMouseFunc(_mouseFuncWrapper);
    glutMotionFunc(_motionFuncWrapper);
    //glutMouseWheelFunc(mouseWheel);
}

void Controls::Mouse::_mouse(int button, int state, int x, int y)
{
    _lastlastx = _lastx;
    _lastlasty = _lasty;
    _lastx=x;
    _lasty=y;
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        _buttons[0] = ((GLUT_DOWN==state)? true : false);
        //std::cout << "Left button pressed." << x << ", " << y << std::endl;
        break;

    case GLUT_MIDDLE_BUTTON:
        _buttons[1] = ((GLUT_DOWN==state)? true : false);
        //std::cout << "Middle button pressed." << x << ", " << y << std::endl;
        break;

    case GLUT_RIGHT_BUTTON:
        _buttons[2] = ((GLUT_DOWN==state)? true : false);
        //std::cout << "Right button pressed." << x << ", " << y << std::endl;
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

void Controls::Mouse::_motion(int x, int y)
{
    int diffxlast = _lastx - _lastlastx;
    int diffylast = _lasty - _lastlasty;
    int diffx=x-_lastx;
    int diffy=y-_lasty;
    _lastlastx = _lastx;
    _lastlasty = _lasty;
    _lastx=x;
    _lasty=y;

    if(_buttons[0])
    {
        _camera->panDown(0.01f * diffy);
        _camera->panRight(0.01f * diffx);
    }
    else if(_buttons[2])
    {
        //_camera->setTz(_camera->getTz() * exp(0.01f*diffy) + 0.0001);
        /*glm::vec3 v((float)diffx, (float)diffy, 0.0f);
        glm::vec3 vlast((float)diffxlast, (float)diffylast, 0.0f);
        float r = glm::cross(v, vlast).z;
        float dC = 0.1f * v.length();
        if (r > 0) dC = -dC;
        _meshObject->collapseTo(_meshObject->complexity() + dC);//*/
    }
    else if(_buttons[1])
    {
        //_camera->setTz(_camera->getTz() * exp(0.01f*diffx) + 0.0001);
        //_camera->setTx(_camera->getTx() + (float) 0.001f * diffx);
        //_camera->setTy(_camera->getTy() - (float) 0.001f * diffy);
    }
    glutPostRedisplay();
}

void Controls::Mouse::_mouseFuncWrapper(int button, int state, int x, int y)
{
    mainMouse->_mouse(button, state, x, y);
}

void Controls::Mouse::_motionFuncWrapper(int x, int y)
{
    mainMouse->_motion(x, y);
}

//TODO Keyboard functions
void Controls::Keyboard::init()
{
    mainKeyboard = this;
    glutKeyboardFunc(Controls::Keyboard::_keyboardFuncWrapper);
    glutSpecialFunc(Controls::Keyboard::_keyboardSpecialFuncWrapper);

    hotkey_map['p'] = exitFunc;

    auto camBack = [&]() {_camera->setPos(_camera->pos() - _camStep*_camera->dir()); };
    auto camForward = [&]() {_camera->setPos(_camera->pos() + _camStep*_camera->dir()); };
    auto camLeft = [&]() {_camera->setPos(_camera->pos() - _camStep*_camera->right()); };
    auto camRight = [&]() {_camera->setPos(_camera->pos() + _camStep*_camera->right()); };
    auto camDown = [&]() {_camera->setPos(_camera->pos() - _camStep*_camera->up()); };
    auto camUp = [&]() {_camera->setPos(_camera->pos() + _camStep*_camera->up()); };
    auto camRollLeft = [&]() {_camera->rollLeft(M_PI/16); };
    auto camRollRight = [&]() {_camera->rollRight(M_PI/16); };

    hotkey_map['s'] = camBack;
    hotkey_map['w'] = camForward;
    hotkey_map['a'] = camLeft;
    hotkey_map['d'] = camRight;
    hotkey_map['c'] = camDown;
    hotkey_map['r'] = camUp;

    hotkey_map['q'] = camRollLeft;
    hotkey_map['e'] = camRollRight;

}

void Controls::Keyboard::register_hotkey(unsigned char key, std::function<void(void)> func)
{
    hotkey_map[key] = func;
}
void Controls::Keyboard::register_specialkey(int key,  std::function<void (void)> func)
{
    specialkey_map[key] = func;
}

void Controls::Keyboard::_keyPress(unsigned char key, int x, int y)
{
    auto func = hotkey_map[key];
    if (func != nullptr)
    {
        //std::cout << key << " pressed" << std::endl;
        func();
    }

    glutPostRedisplay();
}

void Controls::Keyboard::_specialPress(int key, int x, int y)
{
    auto func = specialkey_map[key];
    if (func != nullptr)
    {
        //std::cout << key << " pressed" << std::endl;
        func();
    }

    glutPostRedisplay();
}
void Controls::Keyboard::_keyboardFuncWrapper(unsigned char key, int x, int y)
{
    mainKeyboard->_keyPress(key, x, y);
}
void Controls::Keyboard::_motionFuncWrapper(unsigned char key, int x, int y)
{
    mainKeyboard->_motion(x, y);
}

void Controls::Keyboard::_keyboardSpecialFuncWrapper(int key, int x, int y)
{
    mainKeyboard->_specialPress(key, x, y);
}
