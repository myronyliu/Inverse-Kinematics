#ifndef _SCENE_H_
#define _SCENE_H_

#include "stdafx.h"
#include "GlutDraw.h"
#include "Joint.h"
#include "utils.h"

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
    Object() :
        _translation(glm::vec3(0, 0, 0)), _rotation(AxisAngleRotation2()), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(true), _objectID(nextID()) {}
    Object(const glm::vec3& translation, const glm::vec3& w) :
        _translation(translation), _rotation(axisAngleRotation2(w)), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(true), _objectID(nextID()) {}
    Object(const glm::vec3& translation, const glm::vec2& axis, const float& angle) :
        _translation(translation), _rotation(AxisAngleRotation2(axis,angle)), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(true), _objectID(nextID()) {}
    Object(const glm::vec3& translation, const AxisAngleRotation2& axisAngle) :
        _translation(translation), _rotation(axisAngle), _color(glm::vec4(1, 1, 1, 1)), _material(DIFFUSE), _visible(true), _objectID(nextID()) {}
    void draw();
    void draw(Shader*);
    virtual void doDraw() = 0;

    /* getters */
    glm::vec3 translation() const { return _translation; }
    glm::vec3 rotationAxis3() const { return axisAngleRotation3(_rotation._axis,1); }
    glm::vec2 rotationAxis2() const { return _rotation._axis; }
    glm::vec3 rotation3() const { return _rotation.axisAngleRotation3(); }
    glm::mat3 rotationMat() const { return rotationMatrix(_rotation); }
    AxisAngleRotation2 rotation() const { return _rotation; }
    AxisAngleRotation2 rotation2() const { return _rotation; }
    bool getVisible() const { return _visible; }
    World* getWorld() const { return _world; }
    int getID() const { return _objectID; }

    /* setters */
    void setColor(const glm::vec4& color) { _color = color; }
    void setTranslation(const glm::vec3& translation) { _translation = translation; }
    void setRotation(const glm::vec3& w) { _rotation = axisAngleRotation2(w); }
    void setRotationAxis(const glm::vec3& w) { _rotation._axis = axisAngleRotation2(w)._axis; }
    void setRotationAxis(const glm::vec2& rotAxis) { _rotation._axis = rotAxis; }
    void setOrientation(const glm::vec3& z, const glm::vec3& y) { _rotation = axisAngleAlignZY2(z, y); }
    void setVisible(bool visible) { _visible = visible; }
    void setWorld(World * world) { _world = world; }

    /* Single line functions */
    int nextID() { return NEXTID++; }

    ~Object() { _world->removeObject(this); }

protected:
    World * _world;
    int _objectID;
    glm::vec3 _translation; // translation
    AxisAngleRotation2 _rotation; // rotation axis direction axisAngleRotationd by (theta,phi)
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
        Object(origin, axisAngleAlignZ2(-displacement)), _length(glm::length(displacement)) {}
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
        Object(center, -axisAngleAlignZY2(z, y)), _dimensions(dimensions) {}
    void doDraw();
protected:
    glm::vec3 _dimensions;
};
class AnchoredBox : public Box {
public:
    AnchoredBox() : Box() {}
    AnchoredBox(const glm::vec3& anchor, const glm::vec3& rotation, const glm::vec3& dimensions):
        Box(anchor, rotation, dimensions) {}
    AnchoredBox(const glm::vec3& anchor, const glm::vec3& z, const glm::vec3& y, const glm::vec3& dimensions) :
        Box(anchor, z, y, dimensions) {}
    void doDraw();
private:
};

class Cylinder : public Object {
public:
    Cylinder() {}
    Cylinder(const glm::vec3& center, const glm::vec3& halfAxis, const float& radius) :
        Object(center, -axisAngleAlignZ2(halfAxis)), _radius(radius), _height(2 * glm::length(halfAxis)) {}
    void doDraw() { GlutDraw::drawCylinder(_translation, glm::vec3(0, 0, _height / 2), _radius); }
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

    void doDraw() { GlutDraw::drawSphere(_translation, _r); }

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
    void setParameterization(glm::vec3 (*axisAngleRotation)(const float&))
    {
        _axisAngleRotation = axisAngleRotation;
    }
    void doDraw();
private:
    glm::vec3(*_axisAngleRotation)(const float&);
};

class Arm : public Object
{
public:
    Arm() : Object() {}
    Arm(std::vector<float>& lengths);

    void append(const float& length = 1, const int& type = BALL, const glm::vec3& wLocal = glm::vec3(0, 0, 0));
    void append(const float& length = 1, const int& type = BALL, const glm::vec2& axisLocal = glm::vec2(0, 0), const float& angleLocal = 0);
    void append(const float& length = 1, const int& type = BALL, const AxisAngleRotation2& axisAngleLocal = AxisAngleRotation2());

    // GETTERS
    float localRotationTheta(const int& joint) const { return _localRotations[joint]._axis[0]; }
    float localRotationPhi(const int& joint) const { return _localRotations[joint]._axis[1]; }
    float localRotationAngle(const int& joint) const { return _localRotations[joint]._angle; }
    glm::vec3 tipPosition() const { return _tip; }

    // SETTERS
    void setLocalRotation(const int&, const glm::vec3&);
    void setLocalRotation(const int&, const AxisAngleRotation2&);
    void setLocalRotationAxis(const int&, const glm::vec3&);
    void setLocalRotationAxis(const int&, const glm::vec2&);
    void setLocalRotationTheta(const int&, const float&);
    void setLocalRotationPhi(const int&, const float&);
    void setLocalRotationAngle(const int&, const float&);
    void setRotation(const glm::vec3&);
    void setTranslation(const glm::vec3&);


    float armLength();
    float armReach();

    void updateRotationDerivative(const int& joint = -1);
    void updateGlobalTransforms(const int& index = 0);
    void update(const int& joint = -1) { updateGlobalTransforms(fmax(0, joint)); updateRotationDerivative(joint); }

    arma::mat forwardJacobian_analytic() const;
    arma::mat forwardJacobian_numeric();

    void nudgeTip(const glm::vec3& displacement); // modifies the rotation angles such that (locally) the tip is nudged in the direction of "displacement"

    void printRotations() const;

    void jiggle(const float&, const float&);
    void jiggle(const int& joint, const float&, const float&);

    void doDraw();
private:
    // _t and _w refer to the anchor position and orientation as per usual
    std::vector<int> _types;
    std::vector<AxisAngleRotation2> _localRotations; // RELATIVE rotation axes
    std::vector<float> _lengths; // lenghts of the arms

    std::vector<AxisAngleRotation2> _globalRotations; // GLOBAL rotation axis for each segment
    std::vector<glm::vec3> _globalTranslations;
    glm::vec3 _tip;

    std::vector<glm::mat3> _dR_dTheta; // local derivatives
    std::vector<glm::mat3> _dR_dPhi;
    std::vector<glm::mat3> _dR_dAngle;

    float _radius;
};



}


#endif