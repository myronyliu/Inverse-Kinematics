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
    Object() : _t(glm::vec3(0,0,0)), _w(glm::vec3(0,0,0)), _color(glm::vec4(1,1,1,1)), _visible(true)
        {
            _objectID = nextID();
        }
    Object(glm::vec3 t, glm::vec3 w) : _t(t), _w(w), _color(glm::vec4(1, 1, 1, 1)), _visible(true)
        {
            _objectID = nextID();
        }
    void draw();
    void draw(Shader *);
    virtual void doDraw() = 0;

    /* getters */
    glm::vec3 position() const { return _t; }
    glm::vec3 rotation() const { return _w; }
    bool getVisible() const { return _visible; }
    World* getWorld() const { return _world; }
    int getID() const { return _objectID; }

    /* setters */
    void setColor(const glm::vec4& color) { _color = color; }
    void setPosition(const glm::vec3& t) { _t = t; }
    void setRotation(const glm::vec3& w) { _w = w; }
    void setOrientation(const glm::vec3& zIn, const glm::vec3& yIn) {
        glm::vec3 z = glm::normalize(zIn);
        glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
        glm::vec3 x = glm::cross(y, z);
        glm::mat3 R = glm::inverse(glm::mat3(x, y, z));
        _w = glm::vec3(R[2][3] - R[3][2], R[3][1] - R[1][3], R[1][2] - R[2][1]);
        _w *= acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2) / (2 * sin(glm::length(_w)));
    }
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
    Arrow() : Object(), _origin(glm::vec3(0, 0, 0)), _displacement(glm::vec3(0, 0, 1)) {}
    Arrow(const glm::vec3& origin, const glm::vec3& displacement) : Object(), _origin(origin), _displacement(displacement) {}
    void doDraw();
private:
    glm::vec3 _displacement;
    glm::vec3 _origin;
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

}


#endif