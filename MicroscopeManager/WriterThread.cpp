#include "WriterThread.h"

WriterThread::WriterThread(ProducerThread* producer, ImageManager* img) :
	bufSize_(producer->bufSize),
	img_(img),
	producer_(producer)
{
	id_ = producer_->lastWriterID;
	producer_->lastWriterID++;
	producer_->AddWriterThread(this);

	buf_ = (unsigned char*)VirtualAlloc(NULL, bufSize_, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	metabuf_ = (MICROSCOPE_METADATA*)VirtualAlloc(NULL, sizeof(MICROSCOPE_METADATA), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (img_)
	{
		img_->CreateFile();
	}
	thd_ = std::thread(&WriterThread::WriteLoop, this);
}

WriterThread::~WriterThread()
{
	VirtualFree(buf_, 0, MEM_RELEASE);
	VirtualFree(metabuf_, 0, MEM_RELEASE);

	if (img_)
	{
		img_->CloseFile();
		delete img_;
	}
}

void WriterThread::WaitForThread()
{
	thd_.join();
}

void WriterThread::WriteLoop()
{
	if (img_)
	{
		while (active)
		{
			{
				std::unique_lock<std::mutex> ul(producer_->pMutex);
				if (!producer_->bufReadys[id_])
				{
					producer_->pCV.wait(ul, [this]() {return producer_->bufReadys[id_]; });
				}
				if (!active)
				{
					return;
				}
				std::memcpy(buf_, producer_->imgBuffers[id_], bufSize_);
				std::memcpy(metabuf_, producer_->metaBuffers[id_], sizeof(MICROSCOPE_METADATA));
				producer_->bufReadys[id_] = false;
			}
			producer_->pCV.notify_all();
			img_->WriteFile(producer_->imgBuffers[id_], producer_->bufSize, producer_->metaBuffers[id_]/*, sizeof(MICROSCOPE_METADATA)*/);
		}
	}
}