#pragma once

#include <string>
#include "MMThread.h"

#undef CreateFile

class ImageManager
{
public:
	virtual ~ImageManager();
	virtual void ProcessData() = 0;
	virtual void CompressData() = 0;
	virtual void CreateFile()= 0;
	virtual void OpenFile() = 0;
	virtual void CloseFile() = 0;
	virtual void WriteFile(unsigned char* buf, unsigned long long writeSize, MICROSCOPE_METADATA* metabuffer, bool newImage = false) = 0;
	virtual void ReadFile(unsigned char* buf, unsigned long long readSize) = 0;
	virtual void SetFilename(std::string filename) = 0;

protected:
	unsigned long long bufferCount_;
};