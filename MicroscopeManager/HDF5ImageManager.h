#pragma once
#include "ImageManager.h"
#include "H5Cpp.h"
#include <vector>
#include "MMThread.h"
#include <H5File.hpp>
#include<memory>
#include "mpi.h"
#include <H5PropertyList.hpp>
#include "H5FDmpio.h"
#include<H5PropertyList_misc.hpp>
#include<H5FDmpi.h>
#include <H5FDmpio.h>
#include <H5FileDriver.hpp>
#include <H5File.hpp>
#include <H5FileDriver_misc.hpp>
#include <H5File_misc.hpp>


class HDF5ImageManager :
    public ImageManager
{
public:
	HDF5ImageManager(std::string filename, float minVol, float maxVol, int slicesPerVol, int volumesPerSecond, int laserMode, float scannerAmplitude, std::vector<char> odorants, unsigned long width, unsigned long height);
	~HDF5ImageManager();
	void ProcessData();
	void CompressData();
	void CreateFile();
	void OpenFile();
	void CloseFile();
	void WriteFile(unsigned char* buf, unsigned long long writeSize, MICROSCOPE_METADATA* metabuffer, bool newImage);
	void ReadFile(unsigned char* buf, unsigned long long readSize);
	void ProcessImage();
	void SetFilename(std::string filename);

private:
	H5::DSetCreatPropList pList_;
	H5::H5File file_;
	H5::Group odorGroup_;
	H5::Group laserGroup_;
	H5::DataSet dataset_;
	H5::DataSet subset_;
	//hid_t datatype_;
	H5::DataSpace dataspace_;
	H5::DataSpace memspace_;
	H5::DataSpace memspaceString_;
	H5::DataSpace attrSpace_;
	H5::DataSpace subSpace_;
	H5::DataSpace attrDimSpace_;

	hid_t fileId_;
	hsize_t dims_[3];
	hsize_t maxDims_[3];
	hsize_t chunkDims_[3];
	hsize_t selectDims_[3];
	hsize_t selectMaxDims_[3];
	hsize_t selectChunkDims_[3];
	hsize_t offset_[3];
	int dimVal[3];

	unsigned char fillVal_;
	//unsigned long long stride_[3];
	//unsigned long long block_[3];
	//herr_t status_;
	std::string filename_;
	std::string sampleInfoVal;
	float minVolVal;
	float maxVolVal;
	float scannerAmplitudeVal;
	int slicesPerVolVal;
	int volPerSecVal;
	int currentOdorVal;
	int previousOdorVal;
	int previousOdorIndex;
	int odorantCount;
	int laserMode;
	int laserModeVal;
	int sliceIndex;
	unsigned long widthVal;
	unsigned long heightVal;
	std::vector<MICROSCOPE_METADATA*> metavector;
	std::vector<char> odorants_;
	/*unsigned long dimVal[3];*/
};

enum laserState { // from arduino buffer
	ONLY_LASER1 = 1,
	ONLY_LASER2 = 2,
	BOTH_ON = 0,
	LASER_INDEX = 0,
	ODOR_INDEX = 1
};
