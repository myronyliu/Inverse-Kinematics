#include "scene.h"

using namespace Scene;
using namespace std;
using namespace glm;
using namespace Math;

/** Global variables **/
int Object::NEXTID = 0;
float jointRadius = 0.1;

// find rotation vector to align object axis with global z-axis


inline glm::vec3 composeRotation(const glm::vec3& w0, const glm::vec3& w1) {
    return axisAngleRotation3(rotationMatrix(w1)*rotationMatrix(w0));
}

/* Method Definitions */

void Camera::pushRotation() const {
    glm::vec3 w = axisAngleAlignVECStoZY3(-_dir, _up);
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
        (180.0f / M_PI)*_rotation._angle,
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
    GlutDraw::drawLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, _length));
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
        glm::vec3 pt = _scale*_parameterization((float)i / n);
        glVertex3f(pt[0], pt[1], pt[2]);
    }
    glEnd();
}


Arm::Arm(std::vector<float>& lengths) :
Object()
{
    int nJoints = lengths.size();
    if (nJoints == 0) return;
    _globalRotations.resize(nJoints + 1);
    _globalTranslations.resize(nJoints + 1);

    _joints.push_back(new BallJoint);
    for (auto length: lengths) {
        _joints.push_back(new BallJoint);
        _joints.back()->setRotation(AxisAngleRotation2());
        _joints.back()->setTranslation(glm::vec3(0, 0, length));
    }

    glm::mat3 R;
    R = rotationMatrix(_rotation);
    _globalTranslations[0] = _translation + R*localTranslation(0);
    glm::vec3 wLocal = localRotation3(0);
    glm::vec3 wLocal_world = R*wLocal;
    R = rotationMatrix(wLocal_world)*R;
    _globalRotations[0] = AxisAngleRotation2(R);
    for (int i = 1; i < nJoints; i++) {
        _globalTranslations[i] = _globalTranslations[i - 1] + R*localTranslation(i);
        wLocal = localRotation3(i);
        wLocal_world = R*wLocal;
        R = rotationMatrix(wLocal_world)*R;
        _globalRotations[i] = AxisAngleRotation2(R);
    }
    _tip = _globalTranslations.back();// +R*localTranslation(nJoints - 1);
}

void Arm::updateGlobalTransforms(const int& joint) {
    int nJoints = _joints.size();
    if (nJoints == 0) return;
    glm::mat3 R;
    if (joint == 0) {
        R = rotationMatrix(_rotation);
        _globalTranslations[0] = _translation + R*localTranslation(0);
    }
    else {
        R = rotationMatrix(_globalRotations[joint - 1]);
        _globalTranslations[joint] = _globalTranslations[joint - 1] + R*localTranslation(joint);
    }
    glm::vec3 wLocal = localRotation3(joint);
    glm::vec3 wLocal_world = R*wLocal; // wLocal in "world coordinates" (NOT the same as wGlobal)
    R = rotationMatrix(wLocal_world)*R;
    _globalRotations[joint] = AxisAngleRotation2(R);
    for (int i = joint + 1; i < nJoints; i++) {
        _globalTranslations[i] = _globalTranslations[i - 1] + R*localTranslation(i);
        wLocal = localRotation3(i);
        wLocal_world = R*wLocal;
        R = rotationMatrix(wLocal_world)*R;
        _globalRotations[i] = AxisAngleRotation2(R);
    }
    _tip = _globalTranslations.back();// +R*localTranslation(nJoints - 1);
}

void Arm::printRotations() const {
    printf("base:  ");
    printVec3(_rotation.axisAngleRotation3());
    printMat3(_rotation.rotationMatrix());
    for (int i = 0; i < _joints.size(); i++) {
        std::string head = "    :  ";
        std::string number = std::to_string(i);
        head.replace(head.begin(), head.begin() + number.length(), number);
        printf("%s", head.c_str());
        //printVec3(localRotation3(i), false);
        localRotation2(i).print();
        //printf("  | ");
        //printVec3(_globalRotations[i].axisAngleRotation3());
        _globalRotations[i].print();
        printMat3(_globalRotations[i].rotationMatrix());
    }
}

void Arm::append(const Joint& joint, const float& length) {
    Joint* jointPtr = new Joint;
    *jointPtr = joint;
    _joints.push_back(jointPtr);
    _globalRotations.push_back(glm::vec3(0, 0, 0));
    _globalTranslations.push_back(glm::vec3(0, 0, 0));
    update(_joints.size() - 1);
}

void Arm::setLocalRotation(const int& joint, const glm::vec3& wLocal) {
    _joints[joint]->setRotation(wLocal);
    _joints[joint]->clamp();
    update(joint);
}
void Arm::setLocalRotation(const int& joint, const AxisAngleRotation2& axisAngle) {
    if (_joints[joint]->rotation2() == axisAngle) return;
    _joints[joint]->setRotation(axisAngle);
    _joints[joint]->clamp();
    update(joint);
}
void Arm::setLocalRotationAxis(const int& joint, const glm::vec3& axis) {
    glm::vec2 thetaPhi = glm::vec2(acos(axis[2] / glm::length(axis)), atan2(axis[1], axis[0]));
    _joints[joint]->setRotationAxis(thetaPhi);
    _joints[joint]->clamp();
    update(joint);
}
void Arm::setLocalRotationAxis(const int& joint, const glm::vec2& axis) {
    if (axis == _joints[joint]->rotation2()._axis) return;
    _joints[joint]->setRotationAxis(axis);
    _joints[joint]->clamp();
    update(joint);
}
void Arm::setLocalRotationTheta(const int& joint, const float& theta) {
    if (theta == _joints[joint]->rotation2()._axis[0]) return;
    _joints[joint]->setRotationTheta(theta);
    _joints[joint]->clamp();
    update(joint);
}
void Arm::setLocalRotationPhi(const int& joint, const float& phi) {
    if (phi == _joints[joint]->rotation2()._axis[1]) return;
    _joints[joint]->setRotationPhi(phi);
    update(joint);
}
void Arm::setLocalRotationAngle(const int& joint, const float& angle) {
    if (angle == _joints[joint]->rotation2()._angle) return;
    _joints[joint]->setRotationAngle(angle);
    update(joint);
}
void Arm::setLocalTranslation(const int& joint, const glm::vec3& translation) {
    if (translation == _joints[joint]->translation()) return;
    _joints[joint]->setTranslation(translation);
    update(joint);
}
void Arm::setRotation(const glm::vec3& w) {
    _rotation = axisAngleRotation2(w);
    updateGlobalTransforms();
}
void Arm::setTranslation(const glm::vec3& translation) {
    if (_translation == translation) return;
    _translation = translation;
    for (int i = 0; i < _joints.size(); i++) {
        _globalTranslations[i] += translation;
    }
    _tip += translation;
}
void Arm::setLocalTranslationRotation(const int& joint, const glm::vec3& translation, const AxisAngleRotation2& rotation) {
    if (translation == _joints[joint]->translation() && rotation == _joints[joint]->rotation2()) return;
    _joints[joint]->setTranslation(translation);
    _joints[joint]->setRotation(rotation);
    update(joint);
}

float Arm::armLength() {
    float lengthSum = 0;
    for (int i = 0; i < _joints.size(); i++) {
        lengthSum += glm::length(localTranslation(i)) + _joints[i]->reach();
    }
    return lengthSum;
}

float Arm::armReach() {
    float reach = 0;
    return reach;
}

void Arm::doDraw() {
    
    if (_joints.size() == 0) return;

    //for (int i = 0; i < _lengths.size(); i++) {
    //    GlutDraw::drawSphere(_globalTranslations[i], _lengths[i] / 4);
    //    //glm::vec3 center;
    //    //if (i == _lengths.size() - 1) {
    //    //    center = (_globalTranslations[i] + _tip) / 2.0f;
    //    //}
    //    //else {
    //    //    center = (_globalTranslations[i] + _globalTranslations[i + 1]) / 2.0f;
    //    //}
    //    //glm::vec3 halfAxis = center - _globalTranslations[i];
    //    //GlutDraw::drawCylinder(center, halfAxis, _radius);
    //}
    //GlutDraw::drawSphere(_tip, _lengths.back() / 4);

    glPushMatrix();

    for (int i = 0; i < _joints.size(); i++) {

        glm::vec3 trans = _joints[i]->translation();
        float length = glm::length(trans);
        float nextLength;
        if (i == _joints.size() - 1) nextLength = 0;
        else nextLength = glm::length(_joints[i + 1]->translation());

        _joints[i]->draw(fmin(length, nextLength) / 8);

        glTranslatef(trans[0], trans[1], trans[2]);
        localRotation2(i).pushRotation();
    }
    glPopMatrix();
}

void Arm::jiggle() {
    for (auto joint : _joints) {
        joint->perturb();
    }
    update();
}
void Arm::jiggle(const int& joint) {
    _joints[joint]->perturb();
    update(joint);
}

arma::mat Arm::forwardJacobian_numeric() {
    int nJoints = _joints.size();

    std::vector<std::vector<float>> jointParams(nJoints);

    // set the size for the jacobian matrix
    int dof = 0;
    for (int i = 0; i < nJoints; i++) {
        std::vector<float> params = _joints[i]->getParams();
        jointParams[i] = params;
        dof += params.size();
    }
    arma::mat J(3, dof);

    float dParam = M_PI / 256;

    std::vector<AxisAngleRotation2> globalRotations = _globalRotations;
    std::vector<glm::vec3> globalTranslations = _globalTranslations;
    glm::vec3 tip = _tip;

    auto restoreTransforms = [&]() {
        _globalRotations = globalRotations;
        _globalTranslations = globalTranslations;
        _tip = tip;
    };

    glm::vec3 trans;
    AxisAngleRotation2 rot;
    glm::vec3 dTip_dParam;
    auto warn = [&]() {
        if (trans != glm::vec3(0, 0, 0)) {
            std::cout << std::endl;
        }
    };

    auto printTip = [this]() {
        return;
        cout << glm::length(_tip) << " : ";
        printVec3(_tip);
    };

    dof = 0;

    for (int joint = 0; joint < nJoints; joint++) {
        std::vector<float> params = jointParams[joint];
        for (int param = 0; param < params.size(); param++) {
            float oldParam = params[param];

            params[param] = oldParam + dParam;
            
            _joints[joint]->backup();

            _joints[joint]->setParams(params);
            update(joint);
            dTip_dParam = _tip;
            _joints[joint]->restore();

            params[param] = oldParam - dParam;
            _joints[joint]->setParams(params);
            update(joint);
            dTip_dParam -= _tip;
            _joints[joint]->restore();
            dTip_dParam /= 2 * dParam;

            params[param] = oldParam;

            J(0, dof) = dTip_dParam[0];
            J(1, dof) = dTip_dParam[1];
            J(2, dof) = dTip_dParam[2];
            restoreTransforms();
            dof++;
        }
    }
    return J;
}

void Arm::nudgeTip(const glm::vec3& displacement) {
    _color = glm::vec4(1, 1, 1, 1);

    auto jigTip = [this]() {
        jiggle(_joints.size() - 1);
        _color = glm::vec4(1, 0, 0, 1);
    };

    arma::mat forwardJ = forwardJacobian_numeric(); // "forwardJ" for FORWARD kinematics
    arma::mat inverseJ;                             // "inverseJ" for INVERSE kinematics

    if (!arma::pinv(inverseJ, forwardJ)) {
        printf("Not invertible. Jiggling tip\n");
        jigTip();
        return;
    }
    for (int i = 0; i < inverseJ.n_rows; i++) {
        for (int j = 0; j < inverseJ.n_cols; j++) {
            if (!isfinite(inverseJ(i, j))) {
                printf("Nonfinite terms found. Jiggling tip\n");
                jigTip();
                return;
            }
        }
    }

    arma::vec d(3);
    d[0] = displacement[0];
    d[1] = displacement[1];
    d[2] = displacement[2];
    arma::vec dParams = inverseJ*d;
    int dof = 0;

    for (auto joint : _joints) {
        std::vector<float> params = joint->getParams();
        for (int param = 0; param < params.size(); param++) {
            //printf("%f ", dParams[dof]);
            params[param] += dParams(dof);
            dof++;
        }
        cout << endl;
        joint->setParams(params);
    }
    updateGlobalTransforms();
}

void Arm::setTip(const glm::vec3& target) {

    std::vector<AxisAngleRotation2> globalRotations;
    std::vector<glm::vec3> globalTranslations;
    glm::vec3 tip;

    auto stashTransforms = [&]() {
        for (auto joint : _joints) {
            joint->backup();
        }
        globalRotations = _globalRotations;
        globalTranslations = _globalTranslations;
        tip = _tip;
    };
    auto restoreTransforms = [&]() {
        for (auto joint : _joints) {
            joint->restore();
        }
        _globalRotations = globalRotations;
        _globalTranslations = globalTranslations;
        _tip = tip;
    };

    float thresh = armLength() / 1024; // satisfactory threshold for distance from target

    while (true) {
        glm::vec3 disp = target - _tip; // displacement to the target
        float dist = glm::length(disp);

        if (dist < thresh) break;

        glm::vec3 dTip = disp;
        int nAttempts = 0;
        while (true) {
            stashTransforms();
            nudgeTip(dTip);

            if (glm::length(target - _tip) < dist) break;

            restoreTransforms();
            nAttempts++;
            if (nAttempts > 256) return;
            if (fabs(dTip[0]) < 2 * FLT_MIN || fabs(dTip[1]) < 2 * FLT_MIN || fabs(dTip[2]) < 2 * FLT_MIN) {
                return;
            }
            dTip /= 2;
        }
    }
}


void setAnchor(const glm::vec3&) {

}