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
                content = (char *)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

void _check_gl_error(const char *file, int line) {
    GLenum err(glGetError());

    while (err != GL_NO_ERROR) {
        std::string error;

        switch (err) {
        case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
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
template <class T>
T clamp(const T& lowerBound, const T& value, const T& upperBound) {
    if (value < lowerBound) return lowerBound;
    else if (value > upperBound) return upperBound;
}




AxisAngleRotation2::AxisAngleRotation2(const glm::mat3& R) {
    AxisAngleRotation2 axisAngle = axisAngle2(R);
    _axis = axisAngle._axis;
    _angle = axisAngle._angle;
}
AxisAngleRotation2::AxisAngleRotation2(const glm::vec2& axis, const float& angleIn) {
    float theta = axis[0];
    float phi = axis[1];
    float angle = angleIn;
    angle -= (2 * M_PI)*floor(angle / (2 * M_PI));

    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta > M_PI) {
        theta = 2 * M_PI - theta;
        phi += M_PI;
    }
    phi -= (2 * M_PI)*floor(phi / (2 * M_PI));
    _axis[0] = theta;
    _axis[1] = phi;
    _angle = angle;
}
AxisAngleRotation2::AxisAngleRotation2(const float& angleIn, const glm::vec2& axis) {
    float theta = axis[0];
    float phi = axis[1];
    float angle = angleIn;
    angle -= (2 * M_PI)*floor(angle / (2 * M_PI));

    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta > M_PI) {
        theta = 2 * M_PI - theta;
        phi += M_PI;
    }
    phi -= (2 * M_PI)*floor(phi / (2 * M_PI));
    _axis[0] = theta;
    _axis[1] = phi;
    _angle = angle;
}
AxisAngleRotation2::AxisAngleRotation2(const glm::vec3& w) {
    float _angle = glm::length(w);
    _angle -= (2 * M_PI)*floor(_angle / (2 * M_PI));
    _axis[0] = acos(w[2] / _angle);
    _axis[1] = atan2(w[1], w[0]);
}
glm::mat3 AxisAngleRotation2::rotationMatrix() const {
    glm::vec3 wHat = glm::vec3(sin(_axis[0])*cos(_axis[1]), sin(_axis[0])*sin(_axis[1]), cos(_axis[0]));
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    float clampedAngle = _angle - (2 * M_PI)*floor(_angle / (2 * M_PI));
    if (clampedAngle == M_PI) {
        return glm::mat3() + 2.0f * wCross*wCross;
    }
    else {
        return glm::mat3() + sin(_angle)*wCross + (1 - cos(_angle))*wCross*wCross;
    }
}
void AxisAngleRotation2::clamp() {
    _angle -= (2 * M_PI)*floor(_angle / (2 * M_PI));

    _axis[0] -= (2 * M_PI)*floor(_axis[0] / (2 * M_PI));
    if (_axis[0] > M_PI) {
        _axis[0] = 2 * M_PI - _axis[0];
        _axis[1] += M_PI;
    }
    _axis[1] -= (2 * M_PI)*floor(_axis[1] / (2 * M_PI));
}
void AxisAngleRotation2::randPerturbAxis(const float& dzArcLength, const float& dPolar) {

    float randTheta = acos(1 - (1 - cos(dzArcLength))*rand() / RAND_MAX);
    float randPhi = 2 * M_PI*rand() / RAND_MAX;
    float randPolar = dPolar * (2.0f*rand() / RAND_MAX - 1);

    glm::mat3 axes = rotationMatrix();
    glm::vec3 x = axes[0];
    glm::vec3 y = axes[1];
    glm::vec3 z = axes[2];

    glm::vec3 wAlignWorld;
    glm::vec3 zTargetLocal;
    glm::vec3 wAlignTarget;
    if (z[0] <= z[1] && z[0] <= z[2]) {
        axisAngleAlignVectors(z, glm::vec3(1, 0, 0));
        zTargetLocal = glm::vec3(cos(randTheta), sin(randTheta)*cos(randPhi), sin(randTheta)*sin(randPhi));
        wAlignTarget = axisAngleAlignVectors(glm::vec3(1, 0, 0), zTargetLocal);
    }
    else if (z[1] <= z[2] && z[1] <= z[0]) {
        axisAngleAlignVectors(z, glm::vec3(0, 1, 0));
        zTargetLocal = glm::vec3(sin(randTheta)*sin(randPhi), cos(randTheta), sin(randTheta)*cos(randPhi));
        wAlignTarget = axisAngleAlignVectors(glm::vec3(0, 1, 0), zTargetLocal);
    }
    else {
        axisAngleAlignVectors(z, glm::vec3(0, 0, 1));
        zTargetLocal = glm::vec3(sin(randTheta)*cos(randPhi), sin(randTheta)*sin(randPhi), cos(randTheta));
        wAlignTarget = axisAngleAlignVectors(glm::vec3(0, 0, 1), zTargetLocal);
    }
    float alignWorldAngle = glm::length(wAlignWorld);

    // rotate axes to world-axes
    if (alignWorldAngle > 0) {
        glm::vec3 rotAxis = wAlignWorld / alignWorldAngle;
        x = glm::rotate(x, alignWorldAngle, rotAxis);
        y = glm::rotate(y, alignWorldAngle, rotAxis);
        z = glm::rotate(z, alignWorldAngle, rotAxis);
    }

    // rotate axes such that z and zTargetLocal are aligned
    if (fabs(randTheta) > 0) {
        glm::vec3 rotAxis = wAlignTarget / randTheta;
        x = glm::rotate(x, fabs(randTheta), rotAxis);
        y = glm::rotate(y, fabs(randTheta), rotAxis);
        z = glm::rotate(z, fabs(randTheta), rotAxis);
    }

    // rotate x,y-axes about the z-axis
    x = glm::rotate(x, randPolar, z);
    y = glm::rotate(y, randPolar, z);

    // undo the first rotation (note that the first glm::rotate conceptually captures TWO rotations)
    if (alignWorldAngle > 0) {
        glm::vec3 rotAxis = wAlignWorld / alignWorldAngle;
        x = glm::rotate(x, -alignWorldAngle, rotAxis);
        y = glm::rotate(y, -alignWorldAngle, rotAxis);
        z = glm::rotate(z, -alignWorldAngle, rotAxis);
    }

    glm::mat3 new_R = glm::mat3(x, y, z);
    glm::vec3 new_axis = glm::normalize(axisAngle3(new_R));
    if (glm::dot(axis3(), new_axis) < 0) new_axis = -new_axis;

    _axis[0] = acos(new_axis[2]);
    _axis[1] = atan2(new_axis[1], new_axis[0]);

    // No need to reset the angle, since any changes are purely from numerical error
}

glm::mat3 basisChangeMatrix(const glm::mat3& oldBasis, const glm::mat3& newBasis) {
    return glm::inverse(newBasis)*oldBasis;
}

glm::mat3 composeLocalTransforms(const glm::mat3& A, const glm::mat3& B) {
    // A defines a basis, so to convert B's columns (currently in standard basis [xHat,yHat,zHat]) to A's basis
    // we left multiply B by the "basis-change-matrix from standard to A" : A_inv
    // The result is A_inv.B, which after multiplying by the previous transformation A, gives...
    return (glm::inverse(A)*B)*A;
}
glm::vec3 composeLocalRotations(const glm::vec3& w0, const glm::vec3& w1) {
    float angle0 = glm::length(w0);
    float angle1 = glm::length(w1);
    if (angle0 == 0) return w1;
    if (angle1 == 0) return w0;
    glm::mat3 R0 = rotationMatrix(w0);
    glm::vec3 w1_frame0 = R0*w1;
    glm::mat3 R = rotationMatrix(w1_frame0)*R0;
    return axisAngleRotation3(R);
}
AxisAngleRotation2 composeLocalRotations(const AxisAngleRotation2& axisAngle0, const AxisAngleRotation2& axisAngle1) {
    glm::vec3 w = composeLocalRotations(axisAngle0.axisAngleRotation3(), axisAngle1.axisAngleRotation3());
    return AxisAngleRotation2(w);
}

glm::mat3 rotationMatrix(const glm::vec3& w) {

    float theta = glm::length(w);
    theta -= (2 * M_PI)*floor(theta / (2 * M_PI));
    if (theta == 0) return glm::mat3();
    glm::vec3 wHat = w / theta;
    glm::mat3 wCross(
        glm::vec3(0, wHat[2], -wHat[1]),
        glm::vec3(-wHat[2], 0, wHat[0]),
        glm::vec3(wHat[1], -wHat[0], 0));
    if (theta == M_PI) {
        return glm::mat3() + 2.0f * wCross*wCross;
    }
    else {
        return glm::mat3() + sin(theta)*wCross + (1 - cos(theta))*wCross*wCross;
    }
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
    float theta = acos(clamp(-1.0f, (R[0][0] + R[1][1] + R[2][2] - 1) / 2, 1.0f));
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
    return axisAngle3(R);
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
        if (axis[2] >= 0) return glm::vec3(0, 0, 0);
        else return glm::vec3(M_PI, 0, 0);
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
        return (M_PI / 2.0f)*glm::cross(axis, target);
    }
}