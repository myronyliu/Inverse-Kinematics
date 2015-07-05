#include "scene.h"
#include "utils.h"

using namespace Scene;
using namespace std;
using namespace glm;
/** Global variables **/
int Object::NEXTID = 0;
float jointRadius = 0.1;

// find rotation vector to align object axis with global z-axis
glm::vec3 wAlignZ(const glm::vec3& axisIn) {
    if (axisIn[0] == 0 && axisIn[1] == 0) {
        return glm::vec3(0, 0, 0);
    }
    else {
        return (M_PI / 2.0f)*glm::cross(glm::vec3(0, 0, 1), glm::normalize(axisIn));
    }
}

glm::vec3 wAlignZY(const glm::vec3& zIn, const glm::vec3& yIn) {
    glm::vec3 z = glm::normalize(zIn);
    glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
    glm::vec3 x = glm::cross(y, z);
    glm::mat3 R = glm::inverse(glm::mat3(x, y, z));
    float theta = acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2);
    return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
}

/* Method Definitions */

void Object::setOrientation(const glm::vec3& z, const glm::vec3& y) {
    _w = wAlignZY(z, y);
}

void World::addObject(Object * obj)
{
    _objects.push_back(obj);
    obj->setWorld(this);
}

void World::draw()
{
    for (auto &object : _objects)
    {
        auto shader = _shaderMap.find(object->getID());
        if (shader != _shaderMap.end())
        {
            object->draw(_shaderMap[object->getID()]);
        }
        else
        {
            object->draw();
        }
    }
}

void World::removeObject(Object * obj)
{
    auto sameID = [&](Object * object) { return object->getID() == obj->getID();  };
    auto to_remove = std::remove_if(std::begin(_objects), std::end(_objects), sameID);
    _objects.erase(to_remove);
}

void World::assignShader(Object * obj, Shader * shader)
{
    _shaderMap[obj->getID()] = shader;
}
Shader * World::findShader(Object * obj)
{
    return _shaderMap[obj->getID()];
}


void Object::draw()
{
    if (!_visible) return;

    GLfloat color[] = { _color[0], _color[1], _color[2], _color[3] };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

    float theta = glm::length(_w);
    glm::vec3 wHat = _w;
    if (theta>0) wHat = _w / theta;
    theta *= 180.0f / M_PI;

    glPushMatrix();
    glTranslated(_t[0], _t[1], _t[2]);
    if (theta > 0) glRotatef(theta, wHat[0], wHat[1], wHat[2]);

    doDraw();

    glPopMatrix();
}

void Object::draw(Shader * shader)
{
    if (!_visible) return;

    float theta = glm::length(_w);
    glm::vec3 wHat = _w;
    if (theta>0) wHat = _w / theta;
    theta *= 180.0f / M_PI;

    glPushMatrix();
    glTranslated(_t[0], _t[1], _t[2]);
    if (theta > 0) glRotatef(theta, wHat[0], wHat[1], wHat[2]);

    shader->link();
    doDraw();
    shader->unlink();
    glPopMatrix();


    glPopAttrib();
}

void Grid::doDraw()
{
    for (int r = -(_rows / 2); r <= (_rows / 2); r++)
    {
        GlutDraw::drawLine(
            glm::vec3(-(_cols / 2.0f)*_gap, 0, r*_gap),
            glm::vec3((_cols / 2.0f)*_gap, 0, r*_gap));
    }
    for (int c = -(_cols / 2); c <= (_cols / 2); c++)
    {
        GlutDraw::drawLine(
            glm::vec3(c*_gap, 0, -(_rows / 2.0f)*_gap),
            glm::vec3(c*_gap, 0, (_rows / 2.0f)*_gap));
    }
}

Arrow::Arrow(const glm::vec3& origin, const glm::vec3& displacement) : Object() {
    _t = origin;
    _length = glm::length(displacement);
    _w = wAlignZ(displacement);
}

void Arrow::doDraw() {
    GlutDraw::drawLine(glm::vec3(0,0,0),glm::vec3(0,0,_length));
    float alpha = 1.0f / 16;
    GlutDraw::drawCone(glm::vec3(0, 0, (1 - alpha)*_length), _length*alpha / 2, glm::vec3(0, 0, alpha*_length));
}

Cylinder::Cylinder(const glm::vec3& center, const glm::vec3& halfAxis, const float& radius) : Object() {
    _t = center;
    _r = radius;
    _h = 2 * glm::length(halfAxis);
    _w = wAlignZ(halfAxis);
}


Box::Box(const glm::vec3& center, const glm::vec3& z, const glm::vec3& y, const glm::vec3& dimensions) : Object() {
    _t = center;
    _dimensions = dimensions;
    _w = wAlignZY(z, y);
}

AnchoredBox::AnchoredBox(const glm::vec3& anchor, const glm::vec3& rotation, const glm::vec3& dimensions) :
//Box(anchor + glm::rotate(glm::vec3(0, 0, dimensions[2]/2), glm::length(rotation), glm::normalize(rotation)), rotation, dimensions) {}
Box(anchor, rotation, dimensions) {}

AnchoredBox::AnchoredBox(const glm::vec3& anchor, const glm::vec3& z, const glm::vec3& y, const glm::vec3& dimensions) :
//Box(anchor + glm::rotate(glm::vec3(0, 0, dimensions[2]/2), glm::length(wAlignZY(z, y)), glm::normalize(wAlignZY(z, y))), wAlignZY(z, y), dimensions) {}
Box(anchor, z, y, dimensions) {}

void AnchoredBox::doDraw() {
    glm::vec3 half = _dimensions / 2.0f;
    GlutDraw::drawRectangle(glm::vec3(half[0], 0, half[2]), glm::vec3(0, half[1], 0), glm::vec3(0, 0, half[2]));
    GlutDraw::drawRectangle(glm::vec3(-half[0], 0, half[2]), glm::vec3(0, half[1], 0), -glm::vec3(0, 0, half[2]));
    GlutDraw::drawRectangle(glm::vec3(0, half[1], half[2]), glm::vec3(0, 0, half[2]), glm::vec3(half[0], 0, 0));
    GlutDraw::drawRectangle(glm::vec3(0, -half[1], half[2]), glm::vec3(0, 0, half[2]), -glm::vec3(half[0], 0, 0));
    GlutDraw::drawRectangle(glm::vec3(0, 0, _dimensions[2]), glm::vec3(half[0], 0, 0), glm::vec3(0, half[1], 0));
    GlutDraw::drawRectangle(glm::vec3(0, 0, 0), glm::vec3(half[0], 0, 0), -glm::vec3(0, half[1], 0));
}

void Box::doDraw() {
    glm::vec3 half = _dimensions / 2.0f;
    GlutDraw::drawRectangle(glm::vec3(half[0], 0, 0), glm::vec3(0, half[1], 0), glm::vec3(0, 0, half[2]));
    GlutDraw::drawRectangle(-glm::vec3(half[0], 0, 0), glm::vec3(0, half[1], 0), -glm::vec3(0, 0, half[2]));
    GlutDraw::drawRectangle(glm::vec3(0, half[1], 0), glm::vec3(0, 0, half[2]), glm::vec3(half[0], 0, 0));
    GlutDraw::drawRectangle(-glm::vec3(0, half[1], 0), glm::vec3(0, 0, half[2]), -glm::vec3(half[0], 0, 0));
    GlutDraw::drawRectangle(glm::vec3(0, 0, half[2]), glm::vec3(half[0], 0, 0), glm::vec3(0, half[1], 0));
    GlutDraw::drawRectangle(-glm::vec3(0, 0, half[2]), glm::vec3(half[0], 0, 0), -glm::vec3(0, half[1], 0));
}

void ObjGeometry::doDraw()
{
    if (!_geomReady)
    {
        _readGeom();
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &_vertices[0]);
    glNormalPointer(GL_FLOAT, 0, &_normals[0]);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());

    return;
}

// Adopted from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
int ObjGeometry::_readGeom()
{
    std::vector< int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > tempVertices;
    std::vector< glm::vec2 > tempUVs;
    std::vector< glm::vec3 > tempNormals;
    int lineCount = 0;
    int faceCount = 0;
    int vertCount = 0;

    std::ifstream file;
    file.open(_filename, std::ios::in);
    if (!file.is_open())
    {
        std::cout << "Could not open " << _filename << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream linestream(line);
        std::string type;
        if (line.find("v ") == 0)
        {
            glm::vec3 vertex;
            linestream >> type >> vertex.x >> vertex.y >> vertex.z;
            vertex.x = vertex.x;
            vertex.y = vertex.y;
            vertex.z = vertex.z;
            tempVertices.push_back(vertex);
            vertCount++;
        }
        else if (line.find("vn ") == 0)
        {
            glm::vec3 normal;
            linestream >> type >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (line.find("vt ") == 0)
        {
            glm::vec2 uv;
            linestream >> type >> uv.x >> uv.y;
            tempUVs.push_back(uv);
        }
        else if (line.find("f ") == 0)
        {
            int vertexIndex[3], normalIndex[3], uvIndex[3];
            char delim;
            linestream >> type >>
                vertexIndex[0] >> delim >> uvIndex[0] >> delim >> normalIndex[0] >>
                vertexIndex[1] >> delim >> uvIndex[1] >> delim >> normalIndex[1] >>
                vertexIndex[2] >> delim >> uvIndex[2] >> delim >> normalIndex[2];

            for (int i = 0; i < 3; i++)
            {
                vertexIndices.push_back(vertexIndex[i]);
                normalIndices.push_back(normalIndex[i]);
                uvIndices.push_back(uvIndex[i]);
            }
            faceCount++;
        }

        lineCount++;
        //if (lineCount % 1000 == 0)
        //{
        //    std::cout << "Parsing obj line: " << lineCount << "\r";
        //}
    }
    std::cout << "Parsed " << lineCount << " lines Verts: " << vertCount << " Triangles: " << faceCount << std::endl;
    file.close();

    for (int i = 0; i < vertexIndices.size(); i++)
    {
        int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = tempVertices[vertexIndex - 1];
        _vertices.push_back(vertex);
    }
    for (int i = 0; i < normalIndices.size(); i++)
    {
        int normalIndex = normalIndices[i];
        glm::vec3 normal = tempNormals[normalIndex - 1];
        _normals.push_back(normal);
    }
    for (int i = 0; i < uvIndices.size(); i++)
    {
        int uvIndex = uvIndices[i];
        glm::vec2 uv = tempUVs[uvIndex - 1];
        _uvs.push_back(uv);
    }

    _geomReady = true;

    return lineCount;
}

World & Scene::createWorld()
{
    World * new_world = new World();
    return *new_world;
}

void Shader::_initShaders()
{
    if (_vertfile == "" || _fragfile == "")
    {
        std::cout << "No shaders! Initialization failing." << std::endl;
        return;
    }
    else if (_shaderReady)
    {
        std::cout << "Shader has already initialized." << std::endl;
        return;
    }

    char *vs, *fs;

    if (_vertfile == "" && _fragfile == ""){ return; }
    _program = glCreateProgram();

    if (_vertfile != "")
    {
        _vertex = glCreateShader(GL_VERTEX_SHADER);
        vs = textFileRead(_vertfile.c_str());
        const char * vv = vs;
        glShaderSource(_vertex, 1, &vv, NULL);
        free(vs);
        glCompileShader(_vertex);
        if (_checkShaderError(_vertex))
        {
            std::cout << _vertfile << " compiled successfully." << std::endl;
            glAttachShader(_program, _vertex);
        }
    }
    if (_fragfile != "")
    {
        _frag = glCreateShader(GL_FRAGMENT_SHADER);
        fs = textFileRead(_fragfile.c_str());
        const char * ff = fs;
        glShaderSource(_frag, 1, &ff, NULL);
        free(fs);
        glCompileShader(_frag);
        if (_checkShaderError(_frag))
        {
            std::cout << _fragfile << " compiled successfully." << std::endl;
            glAttachShader(_program, _frag);
        }
    }

    glLinkProgram(_program);

    glDetachShader(_program, _vertex);
    glDetachShader(_program, _frag);
    glDeleteShader(_vertex);
    glDeleteShader(_frag);

    _shaderReady = true;
    return;
}

bool Shader::_checkShaderError(GLuint shader)
{
    GLint result = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_TRUE) return true;

    GLint logsize = 0;
    char * log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
    log = (char *)malloc(logsize + 1);
    glGetShaderInfoLog(shader, logsize, &result, log);

    std::cout << log << std::endl;
    return false;
}

void Shader::link()
{
    glUseProgram(_program);
}

void Shader::unlink()
{
    glUseProgram(0);
}





void Path::doDraw() {
    int n = 1024;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= n; i++) {
        glm::vec3 pt = _parameterization((float)i / n);
        glVertex3f(pt[0], pt[1], pt[2]);
    }
    glEnd();
}

TreeBody* TreeBody::root() {
    TreeBody* treeBody = this;
    while (treeBody->_parent != NULL) { treeBody = treeBody->_parent; }
    return treeBody;
}

void TreeBody::updateGlobalTransform() {
    std::vector<glm::mat3> Rs;
    std::vector<glm::vec3> ts;
    TreeBody* treeBody = this;
    Rs.push_back(rotationMatrix(_w));
    ts.push_back(_t);
    while (treeBody->_parent != NULL) {
        treeBody = treeBody->_parent;
        Rs.push_back(rotationMatrix(treeBody->_w));
        ts.push_back(_t);
    }
    glm::mat3 R = Rs.back();
    glm::vec3 t = ts.back();
    for (int i = Rs.size() - 2; i > -1; i--) {
        t += R* ts[i];
        R = Rs[i] * R;
    }
    _object->setRotation(angleAxisVector(R));
    _object->setTranslation(t);
}

void TreeBody::doDraw() {
    _object->doDraw();

    glm::vec3 trans = _object->translation();
    glm::vec3 rot = _object->rotation();
    float theta = glm::length(rot);
    rot /= theta;
    theta *= 180.0f / M_PI;

    glPushMatrix();

    glTranslatef(trans[0], trans[1], trans[2]);
    glRotatef(theta, rot[0], rot[1], rot[2]);

    if (_jointType == BALL) {
        GlutDraw::drawSphere(_t, 0.1);
    }
    if (_jointType == PIN) {
        //gl
        //GlutDraw::drawCylinder(_t,)
    }
    glPopMatrix();
}

void Arm::append(const float& length, const int& type, const glm::vec3& wLocal) {
    _lengths.push_back(length);
    _types.push_back(type);
    _wLocals.push_back(wLocal);
    _wGlobals.push_back(glm::vec3(0, 0, 0));
    _tGlobals.push_back(glm::vec3(0, 0, 0));
    updateGlobalTransform(_lengths.size() - 1);
}

void Arm::updateGlobalTransform(const int& index) {
    glm::mat3 R = rotationMatrix(_w); // start with anchor
    glm::vec3 t = _t;
    for (int i = 0; i < index; i++) {
        t += R*glm::vec3(0, 0, _lengths[i]);
        R = R* rotationMatrix(_wLocals[i]);
    }
    _tGlobals[index] = t;
    _wGlobals[index] = angleAxisVector(R);
}

float Arm::armLength() {
    float lengthSum = 0;
    for (int i = 0; i < _lengths.size(); i++) {
        lengthSum += _lengths[i];
    }
    return lengthSum;
}

float Arm::armReach() {
    float reach = 0;
    return reach;
}

void Arm::doDraw() {
    for (int i = 0; i < _lengths.size(); i++) {
        AnchoredBox(_tGlobals[i], _wGlobals[i], glm::vec3(0.125, 0.125, 1)*_lengths[i]).draw();
    }
}