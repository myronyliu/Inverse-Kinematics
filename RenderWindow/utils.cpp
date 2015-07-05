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

glm::mat3 rotationMatrix(const glm::vec3& w) {
    glm::mat3 wCross(
        glm::vec3(0, w[2], -w[1]),
        glm::vec3(-w[2], 0, w[0]),
        glm::vec3(w[1], -w[0], 0));
    float theta = glm::length(w);
    return glm::mat3() + sin(theta)*wCross + (1 - cos(theta))*wCross*wCross;
}

glm::vec3 angleAxisVector(const glm::mat3& R) {
    float theta = acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2);
    return glm::vec3(R[2][3] - R[3][2], R[3][1] - R[1][3], R[1][2] - R[2][1]) / (2 * sin(theta));
}

glm::vec3 angleAxisVector(const glm::vec3& zIn, const glm::vec3& yIn) {
    glm::vec3 z = glm::normalize(zIn);
    glm::vec3 y = glm::normalize(yIn - glm::dot(yIn, z)*z);
    glm::vec3 x = glm::cross(y, z);
    glm::mat3 R = glm::inverse(glm::mat3(x, y, z));
    float theta = acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2);
    return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
}