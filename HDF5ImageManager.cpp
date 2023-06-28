#include "HDF5ImageManager.h"

HDF5ImageManager::HDF5ImageManager(std::string filename, float minVol, float maxVol, int slicesPerVol, int volumesPerSecond, int laserMode, float scannerAmplitude, std::vector<char> odorants, unsigned long width, unsigned long height) :
	filename_(filename),
	slicesPerVolVal(slicesPerVol),
	minVolVal(minVol),
	maxVolVal(maxVol),
	volPerSecVal(volumesPerSecond),
	scannerAmplitudeVal(scannerAmplitude),
	widthVal(width),
	heightVal(height),
	laserModeVal(laserMode),
	fillVal_(0),
	odorants_(odorants)
	
{
	dims_[0] = height;
	dims_[1] = width;
	dims_[2] = 1;

	maxDims_[0] = H5S_UNLIMITED;
	maxDims_[1] = H5S_UNLIMITED;
	maxDims_[2] = H5S_UNLIMITED;

	memspace_ = H5::DataSpace(3, dims_, maxDims_);

	chunkDims_[0] = height;
	chunkDims_[1] = width;
	chunkDims_[2] = 1;

	offset_[0] = 0;
	offset_[1] = 0;
	offset_[2] = 0;

	attrSpace_ = H5::DataSpace(H5S_SCALAR);
	sliceIndex = 0;
	odorantCount = odorants.size();

	dimVal[0] = height;
	dimVal[1] = width;
	dimVal[2] = slicesPerVolVal + 1;
	

	//attrDimSpace_ = H5::DataSpace(3, dimVal, maxDims_);
	
}

HDF5ImageManager::~HDF5ImageManager()
{
	CloseFile();
}

void HDF5ImageManager::ProcessData() // geometrical correction
{}

void HDF5ImageManager::CompressData() // lossless compression
{
}

void HDF5ImageManager::CreateFile()
{

	/*if (odorantCount == 0) {
		throw("no odorant count");
	}*/

	if (filename_.find(".h5") == std::string::npos)
	{
			filename_ += ".h5";
	}

	fileId_ = H5Pcreate(H5P_FILE_ACCESS);

	file_ = H5::H5File(filename_, H5F_ACC_TRUNC);
	pList_.setChunk(3, chunkDims_);
	pList_.setFillValue(H5::PredType::NATIVE_UCHAR, &fillVal_);

	auto fapl = HighFive::FileAccessProps();
	MPI_Comm comm = MPI_COMM_WORLD;
	fapl.add(comm);
	HighFive::File newFile = HighFive::File(filename_, 0U, fapl);
	
	int laserCount;
	

	if (laserModeVal == laserState::BOTH_ON) {
		laserCount = 2;
	}else if (laserModeVal == laserState::ONLY_LASER1 || laserModeVal == laserState::ONLY_LASER2) {
		laserCount = 1;
	}else {
		laserCount = 0;
	}
	
	
	dimVal[0] = heightVal;
	dimVal[1] = widthVal;
	dimVal[2] = slicesPerVolVal + 1;

	std::vector<int> dim_vector;
	dim_vector.push_back(heightVal);
	dim_vector.push_back(widthVal);
	dim_vector.push_back(slicesPerVolVal);

	const int RANK = 2;
	hsize_t dims[1];
	dims[0] = 3;
	//dims[2] = 1;
	H5::DataSpace att_dataspc(1, dims);

	//BASELINE DATASET
	file_.createGroup("BASELINE");
	for (int i = 0; i < laserCount; i++) {
		file_.openGroup("BASELINE").createGroup("LASER " + std::to_string(i));
		for (int j = 0; j < slicesPerVolVal; j++) {
			H5::DataSet slice = file_.openGroup("BASELINE").openGroup("LASER " + std::to_string(i)).createDataSet("SLICE " + std::to_string(j), H5::PredType::NATIVE_UCHAR, memspace_, pList_);
			slice.createAttribute("FRAMES PER VOLUME", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &slicesPerVolVal);
			slice.createAttribute("MIN VOLUME SCALE", H5::PredType::NATIVE_FLOAT, attrSpace_).write(H5::PredType::NATIVE_FLOAT, &minVolVal);
			slice.createAttribute("MAX VOLUME SCALE", H5::PredType::NATIVE_FLOAT, attrSpace_).write(H5::PredType::NATIVE_FLOAT, &maxVolVal);
			slice.createAttribute("VOLUMES PER SECOND", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &volPerSecVal);
			slice.createAttribute("WIDTH", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &widthVal);
			slice.createAttribute("HEIGHT", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &heightVal);
			slice.createAttribute("element_size_um", H5::PredType::NATIVE_INT, att_dataspc).write(H5::PredType::NATIVE_INT, &dimVal);
			slice.extend(dims_);
		}
	}
	
	//ODORANT DATASET
	/*if (odorantCount > 0) {*/
		for (int i = 0; i < odorantCount; i++) { 
			file_.createGroup("ODORANT NO " + std::to_string(i));
			for (int j = 0; j < laserCount; j++) { // laserMode
				file_.openGroup("ODORANT NO " + std::to_string(i)).createGroup("LASER " + std::to_string(j));
				for (int k = 0; k < slicesPerVolVal; k++) {
					H5::DataSet slice = file_.openGroup("ODORANT NO " + std::to_string(i)).openGroup("LASER " + std::to_string(j)).createDataSet("SLICE " + std::to_string(k), H5::PredType::NATIVE_UCHAR, memspace_, pList_);
					slice.createAttribute("FRAMES PER VOLUME", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &slicesPerVolVal);
					slice.createAttribute("MIN VOLUME SCALE", H5::PredType::NATIVE_FLOAT, attrSpace_).write(H5::PredType::NATIVE_FLOAT, &minVolVal);
					slice.createAttribute("MAX VOLUME SCALE", H5::PredType::NATIVE_FLOAT, attrSpace_).write(H5::PredType::NATIVE_FLOAT, &maxVolVal);
					slice.createAttribute("VOLUMES PER SECOND", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &volPerSecVal);
					slice.createAttribute("WIDTH", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &widthVal);
					slice.createAttribute("HEIGHT", H5::PredType::NATIVE_INT, attrSpace_).write(H5::PredType::NATIVE_INT, &heightVal);
					slice.createAttribute("element_size_um", H5::PredType::NATIVE_INT, att_dataspc).write(H5::PredType::NATIVE_INT, &dimVal);
					slice.createAttribute("ODORANT", H5::PredType::NATIVE_INT, attrSpace_);
					slice.extend(dims_);
				}
			}
		}
}

void HDF5ImageManager::OpenFile()
{}
	
void HDF5ImageManager::CloseFile()
{}

void HDF5ImageManager::WriteFile(unsigned char* buf, unsigned long long writeSize, MICROSCOPE_METADATA* metabuffer, bool newImage)
{
	// INITIALIZE VARIABLES
	MICROSCOPE_METADATA mb_ = *metabuffer;
	hsize_t sliceOffset[3];
	sliceOffset[0] = 0;
	sliceOffset[1] = 0;	
	hsize_t sliceDim[3];
	sliceDim[0] = widthVal;
	sliceDim[1] = widthVal;
	H5::DataSet slice;

	//MPI FOR PARALLEL WRITING
	int rank, size;
	unsigned char scatteredData;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Scatter(&buf, 1, MPI_UNSIGNED_CHAR, &scatteredData, 1, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
	/*H5::FileCreatPropList::*/
	MPI_Send(&buf, 1, MPI_UNSIGNED_CHAR, 1, 1, MPI_COMM_WORLD);


	if (mb_.currentOdorantIndex == -2) {	// BASELINE RECORDING ON
		slice = file_.openGroup("BASELINE").openGroup("LASER " + std::to_string(mb_.currentLaser)).openDataSet("SLICE " + std::to_string(mb_.planeIndex));
		H5::DataSpace sliceSpace = slice.getSpace();
		hsize_t dims[3];  
		sliceSpace.getSimpleExtentDims(dims, NULL);
		sliceOffset[2] = dims[2]-1;
		dims_[2] = dims[2]+1;
		slice.extend(dims_);
		sliceSpace = slice.getSpace();
		sliceSpace.selectHyperslab(H5S_SELECT_SET, chunkDims_, sliceOffset);
		slice.write(buf, H5::PredType::NATIVE_UCHAR, memspace_, sliceSpace);
	
	}
	else if (mb_.currentOdorantIndex == -1) { // BASELINE RECORDING OFF
	}
	else { // ODORANT ON
		slice = file_.openGroup("ODORANT NO " + std::to_string(mb_.currentOdorantIndex)).openGroup("LASER " + std::to_string(mb_.currentLaser)).openDataSet("SLICE " + std::to_string(mb_.planeIndex));
		H5::DataSpace sliceSpace = slice.getSpace();
		hsize_t dims[3];
		sliceSpace.getSimpleExtentDims(dims, NULL);
		sliceOffset[2] = dims[2] - 1;
		sliceSpace = slice.getSpace();
		sliceSpace.selectHyperslab(H5S_SELECT_SET, chunkDims_, sliceOffset);
		slice.write(buf, H5::PredType::NATIVE_UCHAR, memspace_, sliceSpace);
		dims_[2] = dims[2] + 1;
		slice.extend(dims_); // NEED TO FIX THIS SO THAT LAST DIMENSION IS NOT REDUNDANT
	}
	MPI_Finalize();
}

void HDF5ImageManager::ReadFile(unsigned char* buf, unsigned long long readSize)
{}

void HDF5ImageManager::SetFilename(std::string filename)
{
	filename_ = filename;
}



