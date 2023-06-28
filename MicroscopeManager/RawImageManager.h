#pragma once

#include "ImageManager.h"
#include "windows.h"
#include <string>
#include <vector>
//#include "MMThread.h"

#undef CreateFile

class RawImageManager :
    public ImageManager
{
public:
    RawImageManager(std::string filename, float minVol, float maxVol, int slicesPerVol, int volumesPerSecond, int laserMode, float scannerAmplitude, std::vector<char> odorants, unsigned long width, unsigned long height);
    ~RawImageManager();

    void ProcessData();
    void CompressData();
    void CreateFile();
    void CloseFile();
    void OpenFile();
    void WriteFile(unsigned char* buf, unsigned long long writeSize, MICROSCOPE_METADATA* metabuffer, bool newImage);
    void ReadFile(unsigned char* buf, unsigned long long readSize);
    void SetFilename(std::string filename);

private:
    std::string filename_;
    HANDLE file_;
    bool open_;
    uint32_t fileHeader_[5];
};

