#pragma once
#include "ImageManager.h"
#include "tiffiop.h"
#include <vector>
#include "MMThread.h"

#undef CreateFile

class TIFFImageManager :
    public ImageManager
{
public:
	TIFFImageManager(std::string filename, float minVol, float maxVol, int slicesPerVol, int volumesPerSecond, int laserMode, float scannerAmplitude, std::vector<char> odorants, unsigned long width, unsigned long height);
	~TIFFImageManager();
	void ProcessData();
	void CompressData();
	void CreateFile();
	void OpenFile();
	void CloseFile();
	void WriteFile(unsigned char* buf, unsigned long long writeSize, MICROSCOPE_METADATA* metabuffer, bool newImage);
	void ReadFile(unsigned char* buf, unsigned long long readSize);
	void SetFilename(std::string filename);

private:
	std::string filename_;
	TIFF* file_;
	bool open_;
	uint32_t height_;
	uint32_t width_;
	tdata_t scanlineBuf_;
	bool writing_;
	bool multiImage_;
	uint16_t page_;
};

