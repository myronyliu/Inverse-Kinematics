#include "utils.h"

/* Utility Functions */
char * textFileRead(const char * fn)
{
    FILE * fp;
    char * content = nullptr;

    int count = 0;
    if (fn != nullptr)
    {
        fopen_s(&fp, fn, "rt");
        if (fp != nullptr)
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0)
            {
                content = (char *) malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

void _check_gl_error(const char *file, int line) {
        GLenum err (glGetError());
 
        while(err!=GL_NO_ERROR) {
                std::string error;
 
                switch(err) {
                        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
                        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
                        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
                        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
                }
 
                std::cerr << "GL_" << error.c_str() <<" - "<<file<<":"<<line<<std::endl;
                err=glGetError();
        }
}

std::string zero_padded_name(std::string prefix, int number, int pad)
{
    std::ostringstream name;
    name << prefix << std::setfill('0') << std::setw(pad) << number;
    std::setfill(' ');

    return name.str();
}


AxisAngleRotation2::AxisAngleRotation2(const glm::mat3& R) {
    AxisAngleRotation2 axisAngle = axisAngle2(R);
    _axis = axisAngle._axis;
    _angle = axisAngle._angle;
}




glm::mat3 rotationMatrix(const glm::vec3& w) {
    glm::mat3 wCross(
        glm::vec3(0, w[2], -w[1]),
        glm::vec3(-w[2], 0, w[0]),
        glm::vec3(w[1], -w[0], 0));
    float theta = glm::length(w);
    return glm::mat3() + sin(theta)*wCross + (1 - cos(theta))*wCross*wCross;
}
glm::mat3 rotationMatrix(const AxisAngleRotation2& axisAngle) {
    return rotationMatrix(axisAngle.parameterize3());
}
glm::mat3 rotationMatrix(const glm::vec2& axis, const float& angle) {
    return rotationMatrix(parameterize3(axis, angle));
}
glm::mat3 rotationMatrix(const float& angle, const glm::vec2& axis) {
    return rotationMatrix(parameterize3(axis, angle));
}


glm::vec3 axisAngle3(const glm::mat3& R) {
    float theta = acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2);
    if (theta == 0) return glm::vec3(0, 0, 0);
    return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
}
AxisAngleRotation2 axisAngle2(const glm::mat3& R) {
    return parameterize2(axisAngle3(R));
}

glm::vec3 axisAngleAlignZY3(const glm::vec3& zIn, const glm::vec3& yIn) {
    glm::vec3 z = glm::normalize(zIn);
    glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
    glm::vec3 x = glm::cross(y, z);
    glm::mat3 R = glm::inverse(glm::mat3(x, y, z));
    float theta = acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2);
    return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
}
AxisAngleRotation2 axisAngleAlignZY2(const glm::vec3& z, const glm::vec3& y) {
    return parameterize2(axisAngleAlignZY3(z, y));
}

AxisAngleRotation2 parameterize2(const glm::vec3& w) {
    float angle = glm::length(w);
    if (angle == 0) return AxisAngleRotation2(glm::vec2(0, 0), 0);
    glm::vec2 axis(acos(w[2] / angle), atan2(w[1], w[0]));
    return AxisAngleRotation2(axis, angle);
}

glm::vec3 parameterize3(const glm::vec2& axis, float angle) {
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[1]));
}
glm::vec3 parameterize3(const float& angle, const glm::vec2& axis) {
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[1]));
}
glm::vec3 parameterize3(const AxisAngleRotation2& axisAngle) {
    glm::vec2 axis = axisAngle._axis;
    float angle = axisAngle._angle;
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[1]));
}

glm::vec3 axisAngleAlignZ3(const glm::vec3& axisIn) {
    if (axisIn[0] == 0 && axisIn[1] == 0) {
        return glm::vec3(0, 0, 0);
    }
    else {
        return (M_PI / 2.0f)*glm::cross(glm::vec3(0, 0, 1), glm::normalize(axisIn));
    }
}
AxisAngleRotation2 axisAngleAlignZ2(const glm::vec3& axis) {
    return parameterize2(axisAngleAlignZ3(axis));
}