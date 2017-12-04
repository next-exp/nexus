#include "HDF5Writer.h"
#include "hdf5_functions.h"
#include <sstream>
#include <cstring>
#include <stdlib.h>

#include <stdint.h>

using namespace nexus;


HDF5Writer::HDF5Writer():
  _file(0), _ievt(0)
{
}

HDF5Writer::~HDF5Writer()
{
}

void HDF5Writer::Open(std::string fileName)
{
  _firstEvent= true;

  _file =  H5Fcreate( fileName.c_str(), H5F_ACC_TRUNC,
                      H5P_DEFAULT, H5P_DEFAULT );

  // Create group for runinfo
  std::string run_group_name = std::string("/Run");
  _rinfoG = createGroup(_file, run_group_name);

  std::string events_table_name = std::string("events");
  _memtypeEvt = createEventType();
  _eventsTable = createTable(_rinfoG, events_table_name, _memtypeEvt);

  _isOpen=true;
}

void HDF5Writer::Close()
{
  _isOpen=false;
  H5Fclose(_file);
}

void HDF5Writer::Write(std::uint64_t timestamp,
                       unsigned int evt_number, size_t run_number)
{
  //Write event number & timestamp
  evt_t evtData;
  evtData.evt_number = evt_number;
  evtData.timestamp = timestamp;
  writeEvent(&evtData, _eventsTable, _memtypeEvt, _ievt);

  _ievt++;
}
