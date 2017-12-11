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

  std::string hit_info_table_name = "hits";
  _memtypeHitInfo = createHitInfoType();
  _hitInfoTable = createTable(_group, hit_info_table_name, _memtypeHitInfo);

  std::string particle_info_table_name = "particles";
  _memtypeParticleInfo = createParticleInfoType();
  _particleInfoTable = createTable(_group, particle_info_table_name, _memtypeParticleInfo);
  
  _isOpen = true;
}

void HDF5Writer::Close()
{
  _isOpen=false;
  H5Fclose(_file);
}

void HDF5Writer::WriteEventInfo(unsigned int evt_number, float evt_energy)
{
  // Write event number 
  evt_t evtData;
  evtData.evt_number = evt_number;
  evtData.evt_energy = evt_energy;
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

void HDF5Writer::WriteHitInfo(int track_indx, int hit_indx, const float* hit_position, float hit_time, float hit_energy, const char* label)
{
  hit_info_t trueInfo;
  trueInfo.hit_indx = hit_indx;
  memcpy(trueInfo.hit_position, hit_position, sizeof(*hit_position));
  trueInfo.hit_time = hit_time;
  trueInfo.hit_energy = hit_energy;
  strcpy(trueInfo.label, label);
  trueInfo.track_indx = track_indx;
  writeHitInfo(&trueInfo,  _hitInfoTable, _memtypeHitInfo, _ihit);
  
  _ihit++;
}

void HDF5Writer::WriteParticleInfo(int track_indx, const char* particle_name, char primary, int pdg_code, const float* initial_vertex, const float* final_vertex, const char* initial_volume, const char* final_volume, const float* momentum, float energy)
{
  particle_info_t trueInfo;
  trueInfo.track_indx = track_indx;
  strcpy(trueInfo.particle_name, particle_name);
  trueInfo.primary = primary;
  trueInfo.pdg_code = pdg_code;
  memcpy(trueInfo.initial_vertex, initial_vertex, sizeof(*initial_vertex));
  memcpy(trueInfo.final_vertex, final_vertex, sizeof(*final_vertex));
  strcpy(trueInfo.initial_volume, initial_volume);
  strcpy(trueInfo.final_volume, final_volume);
  memcpy(trueInfo.momentum, momentum, sizeof(*momentum));
  trueInfo.energy = energy;
  writeParticleInfo(&trueInfo,  _particleInfoTable, _memtypeParticleInfo, _ipart);
  
  _ipart++;
}

// void HDF5Writer::WriteRunInfo(size_t run_number)
// {
//   hsize_t memtype_run = createRunType();
//   std::string run_name = std::string("runInfo");
//   hid_t runinfo_table = createTable(_rinfoG, run_name, memtype_run);
//   runinfo_t runinfo;
//   runinfo.run_number = (int) run_number;
//   writeRun(&runinfo, runinfo_table, memtype_run, 0);

// }

