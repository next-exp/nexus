#include "HDF5Writer.h"
#include "hdf5_functions.h"
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

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

  //_ievt++;
}

void HDF5Writer::WriteSensorDataInfo(unsigned int id, const std::vector< std::pair<unsigned int, float> >& data)
{
  sns_data_t snsData;
  for (const auto& sample : data) {
    snsData.sensor_id = id;
    snsData.time_bin = sample.first;
    snsData.charge = sample.second;
  }
  
  writeSnsData(&snsData, _snsDataTable, _memtypeSnsData, _ievt);

  _ievt++;
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



void HDF5Writer::CreateStructure()
{
  int total_sipms = 256;

  //Create group
  std::string groupName = std::string("/MCRD");
  hid_t group = createGroup(_file, groupName);

  //Create sipms array
  std::string sipm_name = std::string("sipmMCRD");
  _sipmrd = createWaveforms(group, sipm_name, total_sipms, sipmDatasize);
 

  if(extPmt.size() > 0){
    std::string extPmt_name = std::string("extpmt");
    _extpmtrd = createWaveform(group, extPmt_name, extPmtDatasize);
  }
}
