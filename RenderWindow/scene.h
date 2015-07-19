#ifndef _SCENE_H_
#define _SCENE_H_

#include "stdafx.h"
#include "GlutDraw.h"
#include "utils.h"
#include "Math.h"


namespace Scene
{

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

    void pushRotation() const;

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
    Object(const bool& visible = true) :
        _t(glm::vec3(0, 0, 0)), _w(glm::vec3(0,0,0)), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(visible), _objectID(nextID()) {}
    Object(const glm::vec3& translation, const glm::vec3& w) :
        _t(translation), _w(w), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(true), _objectID(nextID()) {}
    
    void draw();
    void draw(Shader*);
    virtual void doDraw() = 0;

    /* getters */
    glm::vec3 translation() const { return _t; }
    glm::vec3 rotation() const { return _w; }
    glm::mat3 rotationMatrix() const { return Math::rotationMatrix(_w); }
    bool getVisible() const { return _visible; }
    World* getWorld() const { return _world; }
    int getID() const { return _objectID; }

    /* setters */
    void setColor(const glm::vec4& color) { _color = color; }
    void setTranslation(const glm::vec3& translation) { _t = translation; }
    void setRotation(const glm::vec3& w) { _w = w; }
    void setOrientation(const glm::vec3& z, const glm::vec3& y) { _w = Math::axisAngleAlignZYtoVECS3(z, y); }
    void setVisible(bool visible) { _visible = visible; }
    void setWorld(World * world) { _world = world; }

    /* Single line functions */
    int nextID() { return NEXTID++; }

    ~Object() { _world->removeObject(this); }

protected:
    World * _world;
    int _objectID;
    glm::vec3 _t;
    glm::vec3 _w;
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
    Arrow(const glm::vec3& origin, const glm::vec3& displacement) :
        Object(origin, displacement), _length(glm::length(displacement)) {}
    void doDraw();
private:
    float _length;
};

class Box : public Object {
public:
    Box() : Object() {}
    Box(const glm::vec3& center, const glm::vec3& rotation, const glm::vec3& dimensions) :
        Object(center, rotation), _dimensions(dimensions) {}
    Box(const glm::vec3& center, const glm::vec3& z, const glm::vec3& y, const glm::vec3& dimensions) :
        Object(center, Math::axisAngleAlignZYtoVECS3(z, y)), _dimensions(dimensions) {}
    void doDraw();
protected:
    glm::vec3 _dimensions;
};

class Cylinder : public Object {
public:
    Cylinder() {}
    Cylinder(const glm::vec3& center, const glm::vec3& halfAxis, const float& radius) :
        Object(center, Math::axisAngleAlignZtoVEC3(halfAxis)), _radius(radius), _height(2 * glm::length(halfAxis)) {}
    void doDraw() { GlutDraw::drawCylinder(_t, glm::vec3(0, 0, _height / 2), _radius); }
private:
    float _radius;
    float _height;
};

class Sphere : public Object
{
public:
    /* Constructors */
    Sphere() : Object(), _r(5) { }
    Sphere(const glm::vec3& translation, const float& radius) : Object(translation,glm::vec3(0,0,0)), _r(radius) { }

    void doDraw() { GlutDraw::drawSphere(glm::vec3(0, 0, 0), glm::vec3(0, 0, _r)); }

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
    Path(const float& scale = 1, const float& time = 0) : Object(),_scale(scale), _time(time) {}

    void setParameterization(glm::vec3 (*parameterization)(const float&)) { _parameterization = parameterization; }
    void setT(const float& time) { _time = time - floor(time); }
    glm::vec3 stepT(const float& dt = 1.0f / 1024) {
        _time += dt;
        _time -= floor(_time);
        return point(_time);
    }
    glm::vec3 point(const float& t) const {
        glm::vec3 pt = _scale*_parameterization(t);
        return Math::rotate(pt, _w) + _t;
    }
    glm::vec3 currentPoint() const { return _parameterization(_time); }

    void doDraw();
private:
    glm::vec3(*_parameterization)(const float&);
    float _time;
    float _scale;
};

}


#endif