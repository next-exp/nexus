#include "HDF5Writer.h"

#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <stdint.h>
#include <iostream>

using namespace nexus;


HDF5Writer::HDF5Writer():
  _file(0), _ievt(0), _ismp(0)
{
}

HDF5Writer::~HDF5Writer()
{
}

void HDF5Writer::Open(std::string fileName)
{
  _firstEvent= true;

  _file = H5Fcreate( fileName.c_str(), H5F_ACC_TRUNC,
                      H5P_DEFAULT, H5P_DEFAULT );

  std::string group_name = "/Run";
  _group = createGroup(_file, group_name);

  std::string event_table_name = "events";
  _memtypeEvt = createEventType();
  _eventsTable = createTable(_group,  event_table_name, _memtypeEvt);

  std::string sns_data_table_name = "waveforms";
  _memtypeSnsData = createSensorDataType();
  _snsDataTable = createTable(_group, sns_data_table_name, _memtypeSnsData);
  
  _isOpen = true;
}

void HDF5Writer::Close()
{
  _isOpen=false;
  H5Fclose(_file);
}

void HDF5Writer::WriteEventInfo(unsigned int evt_number)
{
  // Write event number 
  evt_t evtData;
  evtData.evt_number = evt_number;
  writeEvent(&evtData, _eventsTable, _memtypeEvt, _ievt);

  _ievt++;
}

void HDF5Writer::WriteSensorDataInfo(unsigned int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_data_t snsData;
  snsData.sensor_id = sensor_id;
  snsData.time_bin = time_bin;
  snsData.charge = charge;
  writeSnsData(&snsData, _snsDataTable, _memtypeSnsData, _ismp);
  _ismp++;
}

void HDF5Writer::WriteRunInfo(size_t run_number)
{
  hsize_t memtype_run = createRunType();
  std::string run_name = std::string("runInfo");
  hid_t runinfo_table = createTable(_rinfoG, run_name, memtype_run);
  runinfo_t runinfo;
  runinfo.run_number = (int) run_number;
  writeRun(&runinfo, runinfo_table, memtype_run, 0);

}

