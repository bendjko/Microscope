#include "MetaBuffer.h"
#include "windows.h"

MetaBuffer::MetaBuffer(long long timestamp, char currentOdor, int planeCount, float volumeRange, char currentLaser, float laserPower, int cameraMode, char sampleInfo[478], int planeIndex, int planeCounted, int halfCycle) :
	timestamp_(timestamp),
	currentOdor_(currentOdor),
	planeCount_(planeCount),
	volumeRange_(volumeRange),
	currentLaser_(currentLaser),
	laserPower_(laserPower),
	cameraMode_(cameraMode),
	sampleInfo_(sampleInfo),
	planeIndex_(planeIndex),
	planeCounted_(planeCounted),
	halfCycle_(halfCycle)
{
	size_ = width_ * height_ * depth_;
	pixels = (unsigned char*)VirtualAlloc(NULL, size_, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

MetaBuffer::MetaBuffer() :
	timestamp_(timestamp),
	currentOdor_(currentOdor),
	planeCount_(planeCount),
	volumeRange_(volumeRange),
	currentLaser_(currentLaser),
	laserPower_(laserPower),
	cameraMode_(cameraMode),
	sampleInfo_(sampleInfo),
	planeIndex_(planeIndex),
	planeCounted_(planeCounted),
	halfCycle_(halfCycle)
{}

ImageBuffer::~ImageBuffer()
{
	VirtualFree(pixels, 0, MEM_RELEASE);
}

void ImageBuffer::SetSize(unsigned long long width, unsigned long long height)
{
	width_ = width;
	height_ = height;
	size_ = width_ * height_ * depth_;
	VirtualFree(pixels, 0, MEM_RELEASE);
	pixels = (unsigned char*)VirtualAlloc(NULL, size_, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void ImageBuffer::SetPixelDepth(unsigned long long depth)
{
	depth_ = depth;
	size_ = width_ * height_ * depth_;
	VirtualFree(pixels, 0, MEM_RELEASE);
	pixels = (unsigned char*)VirtualAlloc(NULL, size_, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

unsigned long long ImageBuffer::GetSize()
{
	return size_;
}