#include "HDF5Writer.h"

#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <stdint.h>
#include <iostream>

using namespace nexus;


HDF5Writer::HDF5Writer():
  _file(0), _irun(0), _ievt(0), _ismp(0),
  _ismp_tof(0), _ihit(0),
  _ipart(0), //_ievt_extent(0),
  _ipos(0)
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

  std::string group_name = "/MC";
  _group = createGroup(_file, group_name);

  std::string run_table_name = "configuration";
  _memtypeRun = createRunType();
  _runTable = createTable(_group, run_table_name, _memtypeRun);

  std::string event_table_name = "events";
  _memtypeEvt = createEventType();
  _eventsTable = createTable(_group, event_table_name, _memtypeEvt);

  std::string sns_data_table_name = "waveforms";
  _memtypeSnsData = createSensorDataType();
  _snsDataTable = createTable(_group, sns_data_table_name, _memtypeSnsData);

  std::string sns_tof_table_name = "tof_waveforms";
  _memtypeSnsTof = createSensorTofType();
  _snsTofTable = createTable(_group, sns_tof_table_name, _memtypeSnsTof);

  std::string hit_info_table_name = "hits";
  _memtypeHitInfo = createHitInfoType();
  _hitInfoTable = createTable(_group, hit_info_table_name, _memtypeHitInfo);

  std::string particle_info_table_name = "particles";
  _memtypeParticleInfo = createParticleInfoType();
  _particleInfoTable = createTable(_group, particle_info_table_name, _memtypeParticleInfo);

  // std::string evt_extent_table_name = "extents";
  // _memtypeEventExtent = createEventExtentType();
  // _evtExtentTable = createTable(_group, evt_extent_table_name, _memtypeEventExtent);

  std::string sns_pos_table_name = "sensor_positions";
  _memtypeSnsPos = createSensorPosType();
  _snsPosTable = createTable(_group, sns_pos_table_name, _memtypeSnsPos);

  _isOpen = true;
}

void HDF5Writer::Close()
{
  _isOpen=false;
  H5Fclose(_file);
}

void HDF5Writer::WriteRunInfo(const char* param_key, const char* param_value)
{

  run_info_t runData;
  memset(runData.param_key,   0, CONFLEN);
  memset(runData.param_value, 0, CONFLEN);
  strcpy(runData.param_key, param_key);
  strcpy(runData.param_value, param_value);
  writeRun(&runData, _runTable, _memtypeRun, _irun);

  _irun++;
}

void HDF5Writer::WriteEventInfo(int evt_number, float evt_energy)
{
  // Write event number
  evt_t evtData;
  evtData.evt_id = evt_number;
  evtData.evt_energy = evt_energy;
  writeEvent(&evtData, _eventsTable, _memtypeEvt, _ievt);

  _ievt++;
}

void HDF5Writer::WriteSensorDataInfo(int evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_data_t snsData;
  snsData.evt_id = evt_number;
  snsData.sensor_id = sensor_id;
  snsData.time_bin = time_bin;
  snsData.charge = charge;
  writeSnsData(&snsData, _snsDataTable, _memtypeSnsData, _ismp);

  _ismp++;
}

void HDF5Writer::WriteSensorTofInfo(int evt_number, int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_tof_t snsTof;
  snsTof.evt_id = evt_number;
  snsTof.sensor_id = sensor_id;
  snsTof.time_bin = time_bin;
  snsTof.charge = charge;
  writeSnsTof(&snsTof, _snsTofTable, _memtypeSnsTof, _ismp_tof);

  _ismp_tof++;
}

void HDF5Writer::WriteHitInfo(int evt_number, int particle_indx, int hit_indx, float hit_position_x, float hit_position_y, float hit_position_z, float hit_time, float hit_energy, const char* label)
{
  hit_info_t trueInfo;
  trueInfo.evt_id = evt_number;
  memset(trueInfo.label, 0, STRLEN);
  //memcpy(trueInfo.hit_position, hit_position, sizeof(*hit_position)*size_position);
  trueInfo.hit_pos_x = hit_position_x;
  trueInfo.hit_pos_y = hit_position_y;
  trueInfo.hit_pos_z = hit_position_z;
  trueInfo.hit_time = hit_time;
  trueInfo.hit_energy = hit_energy;
  strcpy(trueInfo.label, label);
  trueInfo.particle_id = particle_indx;
  trueInfo.hit_id = hit_indx;
  writeHit(&trueInfo,  _hitInfoTable, _memtypeHitInfo, _ihit);

  _ihit++;
}

void HDF5Writer::WriteParticleInfo(int evt_number, int particle_indx, const char* particle_name, char primary, int mother_id, float initial_vertex_x, float initial_vertex_y, float initial_vertex_z, float initial_vertex_t, float final_vertex_x, float final_vertex_y, float final_vertex_z, float final_vertex_t, const char* initial_volume, const char* final_volume, float momentum_x, float momentum_y, float momentum_z, float kin_energy, const char* creator_proc)
{
  particle_info_t trueInfo;
  trueInfo.evt_id = evt_number;
  trueInfo.particle_id = particle_indx;
  memset(trueInfo.particle_name, 0, STRLEN);
  strcpy(trueInfo.particle_name, particle_name);
  trueInfo.primary = primary;
  trueInfo.mother_id = mother_id;
  trueInfo.initial_vertex_x = initial_vertex_x;
  trueInfo.initial_vertex_y = initial_vertex_y;
  trueInfo.initial_vertex_z = initial_vertex_z;
  trueInfo.initial_vertex_t = initial_vertex_t;
  trueInfo.final_vertex_x = final_vertex_x;
  trueInfo.final_vertex_y = final_vertex_y;
  trueInfo.final_vertex_z = final_vertex_z;
  trueInfo.final_vertex_t = final_vertex_t;
  // memcpy(trueInfo.initial_vertex, initial_vertex, sizeof(*initial_vertex)*size_initial_vertex);
  //  memcpy(trueInfo.final_vertex, final_vertex, sizeof(*final_vertex)*size_final_vertex);
  memset(trueInfo.initial_volume, 0, STRLEN);
  strcpy(trueInfo.initial_volume, initial_volume);
  memset(trueInfo.final_volume, 0, STRLEN);
  strcpy(trueInfo.final_volume, final_volume);
  trueInfo.momentum_x = momentum_x;
  trueInfo.momentum_y = momentum_y;
  trueInfo.momentum_z = momentum_z;
  //  memcpy(trueInfo.momentum, momentum, sizeof(*momentum)*size_momentum);
  trueInfo.kin_energy = kin_energy;
  memset(trueInfo.creator_proc, 0, STRLEN);
  strcpy(trueInfo.creator_proc, creator_proc);
  writeParticle(&trueInfo,  _particleInfoTable, _memtypeParticleInfo, _ipart);

  _ipart++;
}

// void HDF5Writer::WriteEventExtentInfo(int evt_number, unsigned int last_sns_data, unsigned int last_sns_tof, unsigned int last_hit, unsigned int last_particle)
// {
//   // Write event number
//   evt_extent_t evtData;
//   evtData.evt_number = evt_number;
//   evtData.last_sns_data = last_sns_data;
//   evtData.last_sns_tof = last_sns_tof;
//   evtData.last_hit = last_hit;
//   evtData.last_particle = last_particle;
//   writeEventExtent(&evtData, _evtExtentTable, _memtypeEventExtent, _ievt_extent);

//   _ievt_extent++;
// }

void HDF5Writer::WriteSensorPosInfo(unsigned int sensor_id, float x, float y, float z)
{
  sns_pos_t snsPos;
  snsPos.sensor_id = sensor_id;
  snsPos.x = x;
  snsPos.y = y;
  snsPos.z = z;
  writeSnsPos(&snsPos, _snsPosTable, _memtypeSnsPos, _ipos);

  _ipos++;
}

// size_t HDF5Writer::GetSnsDataIndex() const
// {
//   if (_ismp == 0)
//     return 0;
//   else
//     return _ismp - 1;
// }

// size_t HDF5Writer::GetSnsTofIndex() const
// {
//   if (_ismp_tof == 0)
//     return 0;
//   else
//     return _ismp_tof - 1;
// }

// size_t HDF5Writer::GetHitIndex() const
// {
//   if (_ihit == 0)
//     return 0;
//   else
//     return _ihit - 1;
// }

// size_t HDF5Writer::GetParticleIndex() const
// {
//   if (_ipart == 0)
//     return 0;
//   else
//     return _ipart- 1;
// }
