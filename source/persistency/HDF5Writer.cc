#include "HDF5Writer.h"

#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <stdint.h>
#include <iostream>

using namespace nexus;


HDF5Writer::HDF5Writer():
  _file(0), _ievt(0), _ismp(0), _ihit(0), _ipart(0), _ievt_extent(0)
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

  std::string evt_extent_table_name = "extents";
  _memtypeEventExtent = createEventExtentType();
  _evtExtentTable = createTable(_group, evt_extent_table_name, _memtypeEventExtent);
  
  _isOpen = true;
}

void HDF5Writer::Close()
{
  _isOpen=false;
  H5Fclose(_file);
}

void HDF5Writer::WriteEventInfo(int evt_number, float evt_energy)
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

void HDF5Writer::WriteHitInfo(int track_indx, int hit_indx, const float* hit_position, int size_position, float hit_time, float hit_energy, const char* label)
{
  hit_info_t trueInfo;
  trueInfo.hit_indx = hit_indx;
  memcpy(trueInfo.hit_position, hit_position, sizeof(*hit_position)*size_position);
  trueInfo.hit_time = hit_time;
  trueInfo.hit_energy = hit_energy;
  strcpy(trueInfo.label, label);
  trueInfo.track_indx = track_indx;
  writeHit(&trueInfo,  _hitInfoTable, _memtypeHitInfo, _ihit);
  
  _ihit++;
}

void HDF5Writer::WriteParticleInfo(int track_indx, const char* particle_name, char primary, const float* initial_vertex, int size_initial_vertex, const float* final_vertex, int size_final_vertex, const char* initial_volume, const char* final_volume, const float* momentum, int size_momentum, float kin_energy, const char* creator_proc)
{
  particle_info_t trueInfo;
  trueInfo.track_indx = track_indx;
  strcpy(trueInfo.particle_name, particle_name);
  trueInfo.primary = primary;
  memcpy(trueInfo.initial_vertex, initial_vertex, sizeof(*initial_vertex)*size_initial_vertex);
  memcpy(trueInfo.final_vertex, final_vertex, sizeof(*final_vertex)*size_final_vertex);
  strcpy(trueInfo.initial_volume, initial_volume);
  strcpy(trueInfo.final_volume, final_volume);
  memcpy(trueInfo.momentum, momentum, sizeof(*momentum)*size_momentum);
  trueInfo.kin_energy = kin_energy;
  strcpy(trueInfo.creator_proc, creator_proc);
  writeParticle(&trueInfo,  _particleInfoTable, _memtypeParticleInfo, _ipart);
  
  _ipart++;
}

void HDF5Writer::WriteEventExtentInfo(int evt_number, unsigned int init_sns_data, unsigned int init_hit, unsigned int init_particle)
{
  // Write event number 
  evt_extent_t evtData;
  evtData.evt_number = evt_number;
  evtData.init_sns_data = init_sns_data;
  evtData.init_hit = init_hit;
  evtData.init_particle = init_particle;
  writeEventExtent(&evtData, _evtExtentTable, _memtypeEventExtent, _ievt_extent);

  _ievt_extent++;
}
