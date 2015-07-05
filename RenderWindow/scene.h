#ifndef _SCENE_H_
#define _SCENE_H_

#include "stdafx.h"
#include "GlutDraw.h"

namespace Scene
{
enum {
    PIN = 0,
    BALL = 1,
    PRISM = 2
};
enum {
    AMBIENT = 0,
    DIFFUSE = 1
};

class Object;
class Shader;
class TreeBody;

class Camera
{
public:
    Camera() : _pos(glm::vec3(0, 0, 0)), _dir(glm::vec3(0, 0, -1)), _up(glm::vec3(0, 1, 0)) {}

    /* getters */
    glm::vec3 pos() const { return _pos; }
    glm::vec3 dir() const { return _dir; }
    glm::vec3 up() const { return _up; }
    glm::vec3 down() const { return -_up; }
    glm::vec3 right() const { return glm::cross(_dir, _up); }
    glm::vec3 left() const { return -glm::cross(_dir, _up); }

    /* setters */
    void setPos(glm::vec3 pos) { _pos = pos; }
    void setDir(glm::vec3 dir) { _dir = glm::normalize(dir); }
    void setUp(glm::vec3 up) { _up = glm::normalize(up - glm::dot(_up, _dir)*_dir); }

    void panLeft(float theta) { _dir = glm::rotate(_dir, theta, _up); }
    void panRight(float theta) { _dir = glm::rotate(_dir, -theta, _up); }
    void panUp(float theta) { _dir = glm::rotate(_dir, theta, right()); }
    void panDown(float theta) { _dir = glm::rotate(_dir, -theta, right()); }

    void rollLeft(float theta) { _up = glm::rotate(_up, -theta, _dir); }
    void rollRight(float theta) { _up = glm::rotate(_up, theta, _dir); }

    glm::mat4 rotMat() {
        glm::vec4 x(right()[0], right()[1], right()[2], 0);
        glm::vec4 y(up()[0], up()[1], up()[2], 0);
        glm::vec4 z(-_dir[0], -_dir[1], -_dir[2], 0);
        return glm::inverse(glm::mat4(x, y, z, glm::vec4(0, 0, 0, 1)));
    }
    float* rotMatData() {
        glm::mat4 M = rotMat();
        float* out = new float[16];
        for (int i = 0; i < 16; i++) { out[i] = M[i / 4][i % 4]; }
        return out;
    }

private:
    glm::vec3 _pos; // camera position
    glm::vec3 _dir; // camera viewing vector
    glm::vec3 _up;
};

class World
{
public:
    World() : _cam(nullptr) {}

    void addObject(Object *);
    void removeObject(Object *);
    void assignShader(Object *, Shader *);
    Shader * findShader(Object *);

    void addLight(const glm::vec3& position, const glm::vec4& diffuse) {
        glEnable(GL_LIGHT0);
        GLfloat light_position[] = { position[0], position[1], position[2] };
        GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
        GLfloat light_diffuse[] = { diffuse[0], diffuse[1], diffuse[2], diffuse[3] };
        GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }

    //void removeObject(Object & obj) {  }

    Camera * getCam() { return _cam; }

    void draw();

    ~World() {};
private:
    std::vector<Object *> _objects;
    std::unordered_map<int, Shader *> _shaderMap;

    Camera * _cam;
};

World & createWorld();

    /* Base class for vert/frag shader. */
class Shader
{
public:
/* Constructors */
    Shader() : _vertfile(), _fragfile(), _shaderReady(false) { };
    Shader(std::string vertfile, std::string fragfile)
        : _vertfile(vertfile), _fragfile(fragfile), _shaderReady(false)
        {
            _initShaders();
        };

    virtual void link();
    virtual void unlink();
    GLuint getProgram() { return _program; };

/* Destructors */
    ~Shader() { glDeleteProgram(_program); }

private:
    std::string _vertfile, _fragfile;
    GLuint _program;
    GLuint _vertex;
    GLuint _frag;
    bool _shaderReady;

    void _initShaders();
    bool _checkShaderError(GLuint);
};

class Object
{
public:
/* Constructors */
    Object() : _t(glm::vec3(0,0,0)), _w(glm::vec3(0,0,0)), _color(glm::vec4(1,1,1,1)), _material(DIFFUSE), _visible(true)
        {
            _objectID = nextID();
        }
    Object(glm::vec3 t, glm::vec3 w) : _t(t), _w(w), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(true)
        {
            _objectID = nextID();
        }
    void draw();
    void draw(Shader *);
    virtual void doDraw() = 0;

    /* getters */
    glm::vec3 translation() const { return _t; }
    glm::vec3 rotation() const { return _w; }
    bool getVisible() const { return _visible; }
    World* getWorld() const { return _world; }
    int getID() const { return _objectID; }

    /* setters */
    void setColor(const glm::vec4& color) { _color = color; }
    void setTranslation(const glm::vec3& t) { _t = t; }
    void setRotation(const glm::vec3& w) { _w = w; }
    void setOrientation(const glm::vec3& z, const glm::vec3& y);
    void setVisible(bool visible) { _visible = visible; }
    void setWorld(World * world) { _world = world; }

    /* Single line functions */
    int nextID() { return NEXTID++; }

    ~Object() { _world->removeObject(this); }

protected:
    World * _world;
    int _objectID;
    glm::vec3 _t; // translation
    glm::vec3 _w; // angle-axis rotation
    glm::vec4 _color;
    int _material;
    bool _visible;

private:
    static int NEXTID;
};

class Grid : public Object
{
public:
/* Constructors */
    Grid() : Object(), _rows(10), _cols(10), _gap(1.0f) { }
    Grid(int rows, int cols, float gap) : Object(),
        _rows(rows), _cols(cols), _gap(gap) { }

    void doDraw();

private:
    int _rows, _cols;
    float _gap;
};

class Arrow : public Object{
public:
    Arrow() : Object(), _length(1) {}
    Arrow(const glm::vec3& origin, const glm::vec3& displacement);
    void doDraw();
private:
    float _length;
};

class Box : public Object {
public:
    Box() : Object() {}
    Box(const glm::vec3& center, const glm::vec3& rotation, const glm::vec3& dimensions) : Object(center, rotation), _dimensions(dimensions) {}
    Box(const glm::vec3& center, const glm::vec3& z, const glm::vec3& y, const glm::vec3& dimensions);
    void doDraw();
protected:
    glm::vec3 _dimensions;
};
class AnchoredBox : public Box {
public:
    AnchoredBox() : Box() {}
    AnchoredBox(const glm::vec3& anchor, const glm::vec3& rotation, const glm::vec3& dimensions);
    AnchoredBox(const glm::vec3& anchor, const glm::vec3& z, const glm::vec3& y, const glm::vec3& dimensions);
    void doDraw();
private:
};

class Cylinder : public Object {
public:
    Cylinder() {}
    Cylinder(const glm::vec3& center, const glm::vec3& halfAxis, const float& radius);
    void doDraw() { GlutDraw::drawCylinder(_t, glm::vec3(0, 0, _h / 2), _r); }
private:
    float _r;
    float _h;
};

class Sphere : public Object
{
public:
    /* Constructors */
    Sphere() : Object(), _r(5) { }
    Sphere(const glm::vec3& translation, const float& radius) : Object(translation,glm::vec3(0,0,0)), _r(radius) { }

    void doDraw() { GlutDraw::drawSphere(_t, _r); }

protected:
    float _r;
};

class ObjGeometry : public Object
{
public:
    ObjGeometry(std::string filename) : Object() { _filename = filename; };
    void doDraw();

    ~ObjGeometry() {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vertexArrayID);
    }

private:
    bool _geomReady;
    int _readGeom();

    std::string _filename;
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<glm::vec2> _uvs;

    GLuint _vertexArrayID;
};

class Path : public Object
{
public:
    Path() : Object() {};
    void setParameterization(glm::vec3 (*parameterization)(const float&))
    {
        _parameterization = parameterization;
    }
    void doDraw();
private:
    glm::vec3(*_parameterization)(const float&);
};


class TreeBody : public Object
{
public:
    TreeBody() {}
    TreeBody(TreeBody* parent) {}

    TreeBody* root();
    void updateGlobalTransform();
    void doDraw();
protected:
    Object* _object; // _object->w and _object->t contain the GLOBAL transforms

    TreeBody* _parent;
    std::vector<TreeBody*> _children;
    int _jointType; // the type of joint connecting this to its parent

    // _t and _w refer to the LOCAL transform relative to the parent (the root has _t,_w = [0,0,0])
    // To get the GLOBAL transform, access body->_t and _body->_w

    glm::vec3 _tJoint; // relative to the Body on which the joint lies
    glm::vec3 _wJoint;
};

class Arm : public Object
{
public:
    Arm() : Object() {}

    void append(const float& length = 1, const int& type = BALL, const glm::vec3& wLocal = glm::vec3(0, 0, 0));
    void updateGlobalTransform(const int&);
    float armLength();
    float armReach();
    void doDraw();
private:
    // _t and _w refer to the anchor position and orientation as per usual
    std::vector<int> _types;
    std::vector<glm::vec3> _wLocals; // RELATIVE rotation vectors
    std::vector<float> _lengths; // lenghts of the arms

    std::vector<glm::vec3> _wGlobals; // GLOBAL rotation vectors for each segment
    std::vector<glm::vec3> _tGlobals;
};



}


#endif