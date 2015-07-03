#ifndef _SCENE_H_
#define _SCENE_H_

#include "stdafx.h"
#include "GlutDraw.h"

namespace Scene
{
enum{
    BINARY_APPROXIMATION_METHOD = 0,
    MIDPOINT_APPROXIMATION_METHOD = 1,
    QUADRIC_APPROXIMATION_METHOD = 2
};

class Object;
class Shader;

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
    void setDir(glm::vec3 dir) { _dir = dir; }
    void setUp(glm::vec3 up) { _up = up; }

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
    Object() : _tx(0), _ty(0), _tz(0), _phi(0), _the(0), _psi(0), _visible(true)
        {
            _objectID = nextID();
        }
    Object(float tx, float ty, float tz, float phi, float the, float psi) : _tx(tx), _ty(ty), _tz(tz),
        _phi(psi), _the(the), _psi(psi), _visible(true)
        {
            _objectID = nextID();
        }
    void draw();
    void draw(Shader *);
    virtual void doDraw() = 0;

    /* getters */
    float getTx() { return _tx; } const
    float getTy() { return _ty; } const
    float getTz() { return _tz; } const
    float getPhi() { return _phi; } const
    float getThe() { return _the; } const
    float getPsi() { return _psi; } const
    bool getVisible() { return _visible; } const
    World* getWorld() { return _world; } const
    int getID() { return _objectID; } const

    /* setters */
    void setTx(float tx) { _tx = tx; }
    void setTy(float ty) { _ty = ty; }
    void setTz(float tz) { _tz = tz; }
    void setPhi(float phi) { _phi = phi; }
    void setThe(float the) { _the = the; }
    void setPsi(float psi) { _psi = psi; }
    void setVisible(bool visible) { _visible = visible; }
    void setWorld(World * world) { _world = world; }

    /* Single line functions */
    int nextID() { return NEXTID++; }

    ~Object() { _world->removeObject(this); }

protected:
    World * _world;
    int _objectID;
    float _tx, _ty, _tz;
    float _phi, _the, _psi;
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
    Arrow() : Object(), _tail(glm::vec3(0, 0, 0)), _head(glm::vec3(0, 0, 1)), _color(glm::vec4(1, 1, 1, 1)) {}
    Arrow(const glm::vec3& tail, const glm::vec3& head) : Object(), _tail(tail), _head(head), _color(glm::vec4(1, 1, 1, 1)) {}
    Arrow(const glm::vec3& tail, const glm::vec3& head, const glm::vec4& color) : Object(), _tail(tail), _head(head), _color(color) {}
    void doDraw();
private:
    glm::vec4 _color;
    glm::vec3 _head;
    glm::vec3 _tail;
};

class Sphere : public Object
{
public:
    /* Constructors */
    Sphere() : Object(), _r(5), _n(100), _m(100) { }
    Sphere(float radius, int n, int m) : Object(), _r(radius), _n(n), _m(m) { }

    void doDraw();

protected:
    int _n, _m; // number of theta and phi subdivisions respectively
    float _r;
};

class ObjGeometry : public Object
{
public:
    ObjGeometry(std::string filename) : Object() { _filename = filename; };
    void doDraw();

    ~ObjGeometry()
    {
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



}


#endif