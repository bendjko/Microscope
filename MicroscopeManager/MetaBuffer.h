#pragma once
class MetaBuffer
{
public:
	MetaBuffer(long long timestamp,char currentOdor,int planeCount,float volumeRange,char currentLaser,float laserPower,int cameraMode,char sampleInfo[478],int planeIndex,int planeCounted,int halfCycle);
	MetaBuffer();
	~MetaBuffer();
	void SetSize(unsigned long long width, unsigned long long height);
	void SetPixelDepth(unsigned long long depth);
	unsigned long long GetSize();
	unsigned char* pixels;

private:
	int remainingLaserImages;
	int remainingStateImages;
	long long timestamp_;
	char currentOdor_;
	int planeCount_;
	float volumeRange_;
	char currentLaser_;
	float laserPower_;
	int cameraMode_;
	char* sampleInfo_;
	int planeIndex_;
	int planeCounted_;
	int halfCycle_;
};