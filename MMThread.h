#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <string>

struct MICROSCOPE_METADATA
{
	//long long timestamp;
	int currentOdorant;
	int currentOdorantIndex;
	int planeCount;
	float volumeRange;
	int currentLaser;
	int planeIndex;
	int halfCycle;
	int volumesPerSecond;
};

class MMThread
{
public:
	MMThread();
	virtual ~MMThread();
	virtual void WaitForThread() = 0;
	virtual void StopThread();

protected:
	std::atomic_bool active;
};

