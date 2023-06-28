#pragma once
#include "MMThread.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include "MicroscopeManager.h"

class WriterThread;

class ProducerThread :
    public MMThread
{
public:
    ProducerThread(int bufsize,
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
    );
    virtual ~ProducerThread();
    void StartThread();
    void WaitForThread();
    void AddWriterThread(WriterThread* writer);
    void Initialize();

    std::mutex pMutex;
    std::condition_variable pCV;
    int producerIndex;
    std::vector<unsigned char*> imgBuffers;
    std::vector<MICROSCOPE_METADATA*> metaBuffers;
    std::vector<bool> bufReadys;
    MicroscopeManager* mm_;
    //MICROSCOPE_METADATA* md_;
    std::atomic_bool masked;
    int bufSize;
    int lastWriterID;
    
    int stateIndex;

    int odorantIndex;
    int currentOdor;
    int currentLaser;
    int remainingLaserImages;
    int remainingStateImages;
    int laserMode_;
    std::vector<std::pair<char, int>> stateAndDuration_;
    std::vector<char> odorants_;
    int planeCount_;
    int volumesPerSecond_;
    float volumeRange_;
    float volumeRangeMax_;
    float volumeRangeMin_;
    int odorantOn;
    int recordingOn;
    int recordingOff;

private:
    virtual void ProducerLoop();
    std::thread thd_;
    std::vector<WriterThread*> writerThreads_;
   
};


