#include "ProducerThread.h"
#include "WriterThread.h"
#include <chrono>
#include <time.h>

ProducerThread::ProducerThread(int bufsize,
	MicroscopeManager* mm,
	/*MICROSCOPE_METADATA* md,*/
	int planeCount,
	float volumeRangeMin,
	float volumeRangeMax,
	int laserMode,
	char sampleInfo[478],
	int volumesPerSecond,
	std::vector<std::pair<char, int>> stateAndDuration,
	std::vector<char> odorants
) :
	producerIndex(0),
	bufSize(bufsize),
	mm_(mm),
	lastWriterID(0),
	planeCount_(planeCount),
	volumesPerSecond_(volumesPerSecond),
	laserMode_(laserMode),
	stateAndDuration_(stateAndDuration),
	odorants_(odorants),
	remainingLaserImages(planeCount),
	remainingStateImages(0),
	currentLaser(0),
	currentOdor(0),
	odorantIndex(0),
	volumeRangeMax_(volumeRangeMax),
	volumeRangeMin_(volumeRangeMin),
	stateIndex(0)
{
	odorantOn = (char)(atoi("131072") >> 16); // 131072 = odorant on in config file
	recordingOn = (char)(atoi("65536") >> 16);
	recordingOff = (char)(atoi("196608") >> 16); 

	volumeRange_ = volumeRangeMax_ - volumeRangeMin_;
	
}

ProducerThread::~ProducerThread()
{
	{
		std::unique_lock<std::mutex> ul(pMutex);
		for (int i = 0; i < writerThreads_.size(); i++)
		{
			writerThreads_[i]->StopThread();
			bufReadys[i] = true;
		}
	}
	pCV.notify_all();

	for (WriterThread* writer : writerThreads_)
	{
		writer->WaitForThread();
		delete writer;
	}
	writerThreads_.clear();

	for (unsigned char* buf : imgBuffers)
	{
		VirtualFree(buf, 0, MEM_RELEASE);
	}

	for (MICROSCOPE_METADATA* buf : metaBuffers)
	{
		VirtualFree(buf, 0, MEM_RELEASE);
	}
}

void ProducerThread::StartThread()
{
	thd_ = std::thread(&ProducerThread::ProducerLoop, this);
}

void ProducerThread::WaitForThread()
{
	thd_.join();
}

void ProducerThread::AddWriterThread(WriterThread* writer)
{
	writerThreads_.push_back(writer);
	imgBuffers.push_back((unsigned char*)VirtualAlloc(NULL, bufSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
	metaBuffers.push_back((MICROSCOPE_METADATA*)VirtualAlloc(NULL, sizeof(MICROSCOPE_METADATA), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
	bufReadys.push_back(false);
}


void ProducerThread::ProducerLoop()
{
	MICROSCOPE_METADATA* metadata = (MICROSCOPE_METADATA*)VirtualAlloc(NULL, 512, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	odorantIndex = 0;
	metadata->planeCount = planeCount_;
	metadata->volumeRange = volumeRange_;
	
	metadata->halfCycle = 0;

	remainingLaserImages = planeCount_;


	if (stateAndDuration_.size() > 0)
	{
		stateIndex = 0;
		remainingStateImages = stateAndDuration_[stateIndex].second * planeCount_ * volumesPerSecond_;
	}
 
	if (laserMode_ == 0) //BOTH LASERS
	{
		currentLaser = 0;
		metadata->currentLaser = currentLaser;
		metadata->planeIndex = 0;

	}
	else if (laserMode_ == 1) // LASER0 ONLY
	{
		metadata->currentLaser = 0;
		metadata->planeIndex = 0;
	}
	else // LASER1 ONLY
	{
		metadata->currentLaser = 1;
		metadata->planeIndex = planeCount_-1;
	}
		
	int change = 0;
	bool newCycle = false;

	while (active)
	{
		{
			std::unique_lock<std::mutex> ul(pMutex);
			if (bufReadys[producerIndex])
			{
				pCV.wait(ul, [this]() {return !bufReadys[producerIndex]; });
			}

			mm_->GetImage();
			memcpy(imgBuffers[producerIndex], mm_->GetImageBuffer(), bufSize);

			//ODORANT INDEX AND ODORANT SETTING
			if (stateAndDuration_[stateIndex].first == recordingOn) {
				metadata->currentOdorantIndex = -2;
				metadata->currentOdorant = -2;
			}
			else if (stateAndDuration_[stateIndex].first == recordingOff){
				metadata->currentOdorantIndex = -1;
				metadata->currentOdorant = -1;
			}
			else {
				metadata->currentOdorantIndex = odorantIndex;
				metadata->currentOdorant = odorants_[odorantIndex];
			}

			memcpy(metaBuffers[producerIndex], metadata, sizeof(MICROSCOPE_METADATA));

			remainingStateImages--;
			// LASER 1 ONLY
			if (laserMode_ == 2) {
				if (metadata->halfCycle % 2 == 0) {
					metadata->planeIndex--;
					if (metadata->planeIndex == 0) {
						metadata->halfCycle++;
					}
				}
				else {
					metadata->planeIndex++;
					if (metadata->planeIndex == planeCount_-1) {
						metadata->halfCycle++;
					}
				}
			}
			// LASER 0 ONLY || BOTH LASERS
			else {
				if (newCycle == true) {
					metadata->halfCycle++;
					newCycle = false;
				}
				else {
					// LASER 0
					if (metadata->halfCycle % 2 == 0) {
						metadata->planeIndex++;
						if (metadata->planeIndex == planeCount_ - 1) {
							newCycle = true;
						}
					}
					// LASER 1
					else {
						metadata->planeIndex--;
						if (metadata->planeIndex == 0) {
							newCycle = true;
						}
					}
				}
			}
			remainingLaserImages--;
			if (remainingLaserImages == 0)
			{
				remainingLaserImages = planeCount_;

				if (laserMode_ == 0) {
					if (currentLaser == 1)
					{
						currentLaser = 0;
					}
					else
					{
						currentLaser = 1;
					}
					metadata->currentLaser = currentLaser;
				}

			}

			// DONE WITH ONE STATE
			if (remainingStateImages == 0)
			{
				stateIndex++;
				if (stateIndex < stateAndDuration_.size()) // 
				{
					remainingStateImages = stateAndDuration_[stateIndex].second * planeCount_ * volumesPerSecond_;

					if (stateAndDuration_[stateIndex-1].first == odorantOn) {
						odorantIndex++;
					}
				}
				// REACHED THE END STATE
				else
				{
					active = false;
				}
			}

			bufReadys[producerIndex] = true;
			producerIndex++;
			
			if (producerIndex == writerThreads_.size())
			{
				producerIndex = 0;
			}
			
		}
		pCV.notify_all();
		}
	}

