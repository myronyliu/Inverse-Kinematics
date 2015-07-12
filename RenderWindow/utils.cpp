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

template<class T>
T* pointerToCopy(const T& original) {
    T* copyLocation = (T*)malloc(sizeof(original));
    memcpy(copyLocation, &original, sizeof(original));
    return copyLocation;
}
template<class T>
T* pointerToCopy(const T* original) {
    T* copyLocation = (T*)malloc(sizeof(*original));
    memcpy(copyLocation, original, sizeof(*original));
    return copyLocation;
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
void printVec2(const glm::vec2& v, const bool& newLine) {
    if (v[0] < 0) printf("%f", v[0]);
    else printf(" %f", v[0]);

    if (v[1] < 0) printf(" %f", v[1]);
    else printf("  %f", v[1]);

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


void printVector(const std::vector<float>& v) {
    if (v.size() == 0) {
        std::cout << std::endl;
        return;
    }
    std::cout << v[0];
    for (int i = 1; i < v.size(); i++) {
        std::cout << ", " << v[i];
    }
    std::cout << std::endl;
}




