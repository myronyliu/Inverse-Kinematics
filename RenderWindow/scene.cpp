#include "scene.h"

using namespace Scene;
using namespace std;
using namespace glm;
/** Global variables **/
int Object::NEXTID = 0;
float jointRadius = 0.1;

// find rotation vector to align object axis with global z-axis


inline glm::vec3 composeRotation(const glm::vec3& w0, const glm::vec3& w1) {
    return axisAngle3(rotationMatrix(w1)*rotationMatrix(w0));
}

/* Method Definitions */

void Camera::pushRotation() const {
    glm::vec3 w = axisAngleAlignZY3(-_dir, _up);
    float angle = glm::length(w);
    if (angle > 0) {
        w /= angle;
        angle *= 180 / M_PI;
        glRotatef(angle, w[0], w[1], w[2]);
    }
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

    float theta = _rotation._axis[0];
    float phi = _rotation._axis[1];

    glPushMatrix();
    glTranslated(_translation[0], _translation[1], _translation[2]);
    glRotatef(
        (180.0f/M_PI)*_rotation._angle,
        sin(theta)*cos(phi),
        sin(theta)*sin(phi),
        cos(theta));

    doDraw();

    glPopMatrix();
}

void Object::draw(Shader * shader)
{
    if (!_visible) return;

    float theta = _rotation._axis[0];
    float phi = _rotation._axis[1];

    glPushMatrix();
    glTranslated(_translation[0], _translation[1], _translation[2]);
    glRotatef(
        (180.0f / M_PI)*_rotation._angle,
        sin(theta)*cos(phi),
        sin(theta)*cos(phi),
        cos(theta));

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



void Arrow::doDraw() {
    GlutDraw::drawLine(glm::vec3(0,0,0),glm::vec3(0,0,_length));
    float alpha = 1.0f / 16;
    GlutDraw::drawCone(glm::vec3(0, 0, (1 - alpha)*_length), _length*alpha / 2, glm::vec3(0, 0, alpha*_length));
}

void AnchoredBox::doDraw() {
    glPushMatrix();
    glTranslatef(0, 0, -_dimensions[2] / 2);
    Box::doDraw();
    glPopMatrix();
}

void Box::doDraw() {
    glm::vec3 half = _dimensions / 2.0f;
    GlutDraw::drawParallelepiped(glm::vec3(0, 0, 0), glm::vec3(half[0], 0, 0), glm::vec3(0, half[1], 0), glm::vec3(0, 0, half[2]));
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


Arm::Arm(std::vector<float>& lengths) :
Object(),
_lengths(lengths),
_localRotations(std::vector<AxisAngleRotation2>(lengths.size())),
_types(std::vector<int>(lengths.size(), BALL)),
_radius(*std::min_element(lengths.begin(), lengths.end()) / 8)
{
    int nJoints = _lengths.size();
    if (nJoints == 0) return;
    _globalRotations.resize(nJoints);
    _globalTranslations.resize(nJoints);
    glm::mat3 R = rotationMat(); // start with the anchor
    for (int i = 0; i < nJoints; i++) {
        if (i == 0) {
            _globalTranslations[i] = _translation;
        }
        else {
            _globalTranslations[i] = _globalTranslations[i - 1] + R*glm::vec3(0, 0, _lengths[i - 1]);
        }
        R = rotationMatrix(_localRotations[i])*R;
        _globalRotations[i] = axisAngle2(R);
    }
    _tip = _globalTranslations.back() + R*glm::vec3(0, 0, _lengths.back());
}


void Arm::append(const float& length, const int& type, const glm::vec3& wLocal) {
    _lengths.push_back(length);
    _types.push_back(type);
    _localRotations.push_back(parameterize2(wLocal));
    _globalRotations.push_back(glm::vec3(0, 0, 0));
    _globalTranslations.push_back(glm::vec3(0, 0, 0));
    updateGlobalTransforms(_lengths.size() - 1);
}
void Arm::append(const float& length, const int& type, const glm::vec2& axisLocal, const float& angleLocal) {
    _lengths.push_back(length);
    _types.push_back(type);
    _localRotations.push_back(AxisAngleRotation2(axisLocal,angleLocal));
    _globalRotations.push_back(glm::vec3(0, 0, 0));
    _globalTranslations.push_back(glm::vec3(0, 0, 0));
    updateGlobalTransforms(_lengths.size() - 1);
}
void Arm::append(const float& length, const int& type, const AxisAngleRotation2& axisAngleLocal) {
    _lengths.push_back(length);
    _types.push_back(type);
    _localRotations.push_back(axisAngleLocal);
    _globalRotations.push_back(glm::vec3(0, 0, 0));
    _globalTranslations.push_back(glm::vec3(0, 0, 0));
    updateGlobalTransforms(_lengths.size() - 1);
}

void Arm::updateGlobalTransforms(const int& index) {
    int nJoints = _lengths.size();
    if (nJoints == 0) return;
    glm::mat3 R;
    if (index == 0) {
        R = rotationMat(); // starting at the anchor
    }
    else {
        R = rotationMatrix(_globalRotations[index - 1]);
    }
    for (int i = index; i < nJoints; i++) {
        if (i == 0) {
            _globalTranslations[i] = _translation;
        }
        else {
            _globalTranslations[i] = _globalTranslations[i - 1] + R*glm::vec3(0, 0, _lengths[i - 1]);
        }
        R = rotationMatrix(_localRotations[i])*R;
        _globalRotations[i] = AxisAngleRotation2(R);
    }
    _tip = _globalTranslations.back() + R*glm::vec3(0, 0, _lengths.back());
}

void Arm::setLocalJointRotation(const int& joint, const glm::vec3& wLocal) {
    _localRotations[joint] = wLocal;
    updateGlobalTransforms(joint);
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
    glPushMatrix();
    for (int i = 0; i < _lengths.size(); i++) {

        float theta = _localRotations[i]._axis[0];
        float phi = _localRotations[i]._axis[0];

        glRotatef(
            (180.0f / M_PI)*_localRotations[i]._angle,
            sin(theta)*cos(phi),
            sin(theta)*sin(phi),
            cos(theta));

        if (_types[i]==BALL) GlutDraw::drawSphere(glm::vec3(0, 0, 0), _radius);
        //if (_types[i] == PIN) GlutDraw::drawCylinder(glm::vec3(0,0,0),,_radius)
        GlutDraw::drawParallelepiped(glm::vec3(0, 0, _lengths[i] / 2), glm::vec3(0, _radius, 0), glm::vec3(_radius, 0, 0), glm::vec3(0, 0, _lengths[i] / 2 - _radius));

        glTranslatef(0, 0, _lengths[i]);
    }
    glPopMatrix();
}