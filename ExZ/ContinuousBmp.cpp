#include "ContinuousBmp.h"
#include <string>
using namespace std;

void ContinuousBmp::Display(GLdouble *bottomLeft, GLdouble *bottomRight, GLdouble *upRight, GLdouble *upLeft) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[count]);
    glBegin(GL_POLYGON);					//texture coordination
    glTexCoord2f(0.0, 0.0);
    glVertex3dv(bottomLeft);
    glTexCoord2f(1.0, 0.0);
    glVertex3dv(bottomRight);
    glTexCoord2f(1.0, 1.0);
    glVertex3dv(upRight);
    glTexCoord2f(0.0, 1.0);
    glVertex3dv(upLeft);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    auto temp = GetTickCount64();		//get new texture after some time which is not too short
    if (temp - lastTick > 80) {
        count++;
        if (count == texture.size()) {
            count = 0;
        }

        lastTick = temp;
    }
}

unsigned char *ContinuousBmp::LoadBitmapFile(std::string &filename, BITMAPINFOHEADER *bitmapInfoHeader) {	//load image to generate texture
    auto filePtr = fopen(filename.c_str(), "rb");				//open image
    if (filePtr == nullptr) {
        return nullptr;
    }

    BITMAPFILEHEADER bitmapFileHeader;

    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);			//get file header
    if (bitmapFileHeader.bfType != BITMAP_ID) {								
        fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file \n");
        return nullptr;
    }

    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // may memory leak
    auto bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
    if (bitmapImage == nullptr) {
        fprintf(stderr, "error in LoadBitmapFile: memory error\n");
        return nullptr;
    }

    fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
    if (bitmapImage == nullptr) {
        fprintf(stderr, "error in LoadBitmapFile: memory error\n");
        return nullptr;
    }

    // convert bgr to rgb
    for (unsigned int imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
        auto tempRgb = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRgb;
    }

    fclose(filePtr);
    return bitmapImage;
}

void ContinuousBmp::LoadTexture(int i) {					//load texture
    string filename = filePath + to_string(i + 1) + ".bmp";
    BITMAPINFOHEADER bitmapInfoHeader;
    auto bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
    glBindTexture(GL_TEXTURE_2D, texture[i]);							//bind texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//define the way to manipulate the texture if
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//the image size doesn't fit

    glTexImage2D(							//specify a 2-D texture image
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        bitmapInfoHeader.biWidth,
        bitmapInfoHeader.biHeight,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        bitmapData
        );
}

void ContinuousBmp::InitTexture() {					// generate all texture
    glGenTextures(texture.size(), &texture[0]);
    for (unsigned int i = 0; i < texture.size(); i++) {		
        LoadTexture(i);
    }
}

ContinuousBmp::ContinuousBmp(GLuint size, char* filePath)	//initialize texture
    :texture(size), filePath(filePath) {
    count = 0;
    lastTick = 0;
}

ContinuousBmp::~ContinuousBmp() {
}