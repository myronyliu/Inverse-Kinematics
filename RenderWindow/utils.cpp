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
void SaveAsBMP(const char *fileName)
{
    FILE *file;
    unsigned long imageSize;
    GLbyte *data = NULL;
    GLint viewPort[4];
    GLenum lastBuffer;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    bmfh.bfType = 'MB';
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = 54;
    glGetIntegerv(GL_VIEWPORT, viewPort);
    imageSize = ((viewPort[2] + ((4 - (viewPort[2] % 4)) % 4))*viewPort[3] * 3) + 2;
    bmfh.bfSize = imageSize + sizeof(bmfh) + sizeof(bmih);
    data = (GLbyte*)malloc(imageSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_SWAP_BYTES, 1);
    glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, viewPort[2], viewPort[3], GL_BGR, GL_UNSIGNED_BYTE, data);
    data[imageSize - 1] = 0;
    data[imageSize - 2] = 0;
    glReadBuffer(lastBuffer);
    std::FILE* f;
    fopen_s(&f, fileName, "wb");
    file = f;
    bmih.biSize = 40;
    bmih.biWidth = viewPort[2];
    bmih.biHeight = viewPort[3];
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;
    bmih.biCompression = 0;
    bmih.biSizeImage = imageSize;
    bmih.biXPelsPerMeter = 45089;
    bmih.biYPelsPerMeter = 45089;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;
    fwrite(&bmfh, sizeof(bmfh), 1, file);
    fwrite(&bmih, sizeof(bmih), 1, file);
    fwrite(data, imageSize, 1, file);
    free(data);
    fclose(file);
}

void printVec3(const glm::vec3& v, const bool& newLine) {
    if (v[0] < 0) printf("%f", v[0]);
    else printf(" %f", v[0]);

    if (v[1] < 0) printf(" %f", v[1]);
    else printf("  %f", v[1]);

    if (v[2] < 0) printf(" %f", v[2]);
    else printf("  %f", v[2]);

    if (newLine) printf("\n");
}
void printMat3(const glm::mat3& M) {

    std::cout << std::endl;

    printf("| ");
    printVec3(glm::vec3(M[0][0], M[1][0], M[2][0]), false);
    printf(" |\n");

    printf("| ");
    printVec3(glm::vec3(M[0][1], M[1][1], M[2][1]), false);
    printf(" |\n");

    printf("| ");
    printVec3(glm::vec3(M[0][2], M[1][2], M[2][2]), false);
    printf(" |\n");

    std::cout << std::endl;
}


AxisAngleRotation2::AxisAngleRotation2(const glm::mat3& R) {
    AxisAngleRotation2 axisAngle = axisAngle2(R);
    _axis = axisAngle._axis;
    _angle = axisAngle._angle;
}
glm::mat3 AxisAngleRotation2::rotationMatrix() const {
    glm::vec3 wHat = glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    return glm::mat3() + sin(_angle)*wCross + (1 - cos(_angle))*wCross*wCross;
}
glm::mat3 AxisAngleRotation2::coordinateAxes() const {
    glm::vec3 wHat = glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    return glm::mat3() + sin(_angle)*wCross + (1 - cos(_angle))*wCross*wCross;
}

void AxisAngleRotation2::perturb(const float& dzArcLength, const float& dPolar) {

    /*printf("%f %f %f\n", sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
    printf("%f\n", _angle);*/

    float randTheta = acos(1 - (1 - cos(dzArcLength))*rand() / RAND_MAX);
    float randPhi = 2 * M_PI*rand() / RAND_MAX;
    float randPolar = dPolar * (2.0f*rand() / RAND_MAX - 1);

    glm::vec3 zTargetLocal = glm::vec3(sin(randTheta)*cos(randPhi), sin(randTheta)*sin(randPhi), cos(randTheta));

    /*printVec3(zTargetLocal);
    std::cout << std::endl;*/


    glm::mat3 axes = coordinateAxes();
    glm::vec3 x = axes[0];
    glm::vec3 y = axes[1];
    glm::vec3 z = axes[2];

    glm::vec3 wAlignZ = axisAngleAlignZ3(z);
    float angle = glm::length(wAlignZ);

    glm::vec3 wAlignTarget = axisAngleAlignVectors(glm::vec3(0, 0, 1), zTargetLocal);

    /*printMat3(glm::mat3(x, y, z));*/

    // rotate axes to world-axes
    if (angle > 0) {
        glm::vec3 rotAxis = wAlignZ / angle;
        x = glm::rotate(x, angle, rotAxis);
        y = glm::rotate(y, angle, rotAxis);
        z = glm::rotate(z, angle, rotAxis);
    }


    /*printf("\n");
    printMat3(glm::mat3(x, y, z));*/

    // rotate axes such that z and zTargetLocal are aligned
    if (fabs(randTheta) > 0) {
        glm::vec3 rotAxis = wAlignTarget / randTheta;
        x = glm::rotate(x, fabs(randTheta), rotAxis);
        y = glm::rotate(y, fabs(randTheta), rotAxis);
        z = glm::rotate(z, fabs(randTheta), rotAxis);
    }

    /*printf("\n");
    printMat3(glm::mat3(x, y, z));*/

    // rotate x,y-axes about the z-axis
    x = glm::rotate(x, randPolar, z);
    y = glm::rotate(y, randPolar, z);

    /*printf("\n");
    printMat3(glm::mat3(x, y, z));*/

    // undo the first rotation (note that the first glm::rotate conceptually captures TWO rotations)
    if (angle > 0) {
        glm::vec3 rotAxis = wAlignZ / angle;
        x = glm::rotate(x, -angle, rotAxis);
        y = glm::rotate(y, -angle, rotAxis);
        z = glm::rotate(z, -angle, rotAxis);
    }

    /*printf("\n");
    printMat3(glm::mat3(x, y, z));*/

    glm::mat3 new_R = glm::mat3(x, y, z);
    AxisAngleRotation2 aa = axisAngle2(new_R);
    _axis = aa._axis;
    //std::cout << fabs(_angle - aa._angle) << std::endl;
    _angle = aa._angle;

    /*printf("%f %f %f\n", sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
    printf("%f\n", _angle);*/
}




glm::mat3 rotationMatrix(const glm::vec3& w) {
    float theta = glm::length(w);
    if (theta == 0) return glm::mat3();
    glm::vec3 wHat = w / theta;
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    return glm::mat3() + sin(theta)*wCross + (1 - cos(theta))*wCross*wCross;
}
glm::mat3 rotationMatrix(const AxisAngleRotation2& axisAngle) {
    return rotationMatrix(axisAngle.axisAngleRotation3());
}
glm::mat3 rotationMatrix(const glm::vec2& axis, const float& angle) {
    return rotationMatrix(axisAngleRotation3(axis, angle));
}
glm::mat3 rotationMatrix(const float& angle, const glm::vec2& axis) {
    return rotationMatrix(axisAngleRotation3(axis, angle));
}


glm::vec3 axisAngle3(const glm::mat3& R) {
    float theta = acos((R[0][0] + R[1][1] + R[2][2] - 1) / 2);
    if (theta == 0) {
        return glm::vec3(0, 0, 0);
    }
    else if (theta == M_PI) {
        if (R[0][0] >= R[1][1] && R[0][0] >= R[2][2]) {
            float x = sqrt((R[0][0] + 1) / 2);
            return theta*glm::vec3(x, R[1][0] / (2 * x), R[2][0] / (2 * x));
        }
        else if (R[1][1] >= R[2][2] && R[1][1] >= R[0][0]) {
            float y = sqrt((R[1][1] + 1) / 2);
            return theta*glm::vec3(R[0][1] / (2 * y), y, R[2][1] / (2 * y));
        }
        else {
            float z = sqrt((R[2][2] + 1) / 2);
            return theta*glm::vec3(R[0][2] / (2 * z), R[1][2] / (2 * z), z);
        }
    }
    else return glm::vec3(R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0])*theta / (2 * sin(theta));
}
AxisAngleRotation2 axisAngle2(const glm::mat3& R) {
    return axisAngleRotation2(axisAngle3(R));
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
    return axisAngleRotation2(axisAngleAlignZY3(z, y));
}

AxisAngleRotation2 axisAngleRotation2(const glm::vec3& w) {
    float angle = glm::length(w);
    if (angle == 0) return AxisAngleRotation2(glm::vec2(0, 0), 0);
    glm::vec2 axis(acos(w[2] / angle), atan2(w[1], w[0]));
    return AxisAngleRotation2(axis, angle);
}

glm::vec3 axisAngleRotation3(const glm::vec2& axis, float angle) {
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}
glm::vec3 axisAngleRotation3(const float& angle, const glm::vec2& axis) {
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}
glm::vec3 axisAngleRotation3(const AxisAngleRotation2& axisAngle) {
    glm::vec2 axis = axisAngle._axis;
    float angle = axisAngle._angle;
    return angle*glm::vec3(sin(axis[0])*cos(axis[1]), sin(axis[0])*sin(axis[1]), cos(axis[0]));
}

glm::vec3 axisAngleAlignZ3(const glm::vec3& axisIn) {
    glm::vec3 axis = glm::normalize(axisIn);
    /*if (axis[2]>0.999999) {
        return glm::vec3(0, 0, 0);
    }*/
    if (axis[0] == 0 && axis[1] == 0) {
        return glm::vec3(0, 0, 0);
    }
    else {
        return (M_PI / 2.0f)*glm::cross(axis, glm::vec3(0, 0, 1));
    }
}

AxisAngleRotation2 axisAngleAlignZ2(const glm::vec3& axis) {
    return axisAngleRotation2(axisAngleAlignZ3(axis));
}

glm::vec3 axisAngleAlignVectors(const glm::vec3& axisIn, const glm::vec3& targetIn) {
    glm::vec3 axis = glm::normalize(axisIn);
    glm::vec3 target = glm::normalize(targetIn);
    /*if (glm::dot(axis, target) > 0.999999) {
        return glm::vec3(0, 0, 0);
    }*/
    if (axis[0] == target[0] && axis[1] == target[1] && axis[2] == target[2]) {
        return glm::vec3(0, 0, 0);
    }
    else {
        return (M_PI / 2.0f)*glm::cross(axis,target);
    }
}