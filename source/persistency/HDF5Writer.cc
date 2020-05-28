#include "HDF5Writer.h"

#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <stdint.h>
#include <iostream>

using namespace nexus;


HDF5Writer::HDF5Writer():
  _file(0), _irun(0), _ismp(0),
  _ismp_tof(0), _ihit(0),
  _ipart(0), _ipos(0)
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

  std::string sns_data_table_name = "sns_response";
  _memtypeSnsData = createSensorDataType();
  _snsDataTable = createTable(_group, sns_data_table_name, _memtypeSnsData);

  std::string sns_tof_table_name = "tof_sns_response";
  _memtypeSnsTof = createSensorTofType();
  _snsTofTable = createTable(_group, sns_tof_table_name, _memtypeSnsTof);

  std::string hit_info_table_name = "hits";
  _memtypeHitInfo = createHitInfoType();
  _hitInfoTable = createTable(_group, hit_info_table_name, _memtypeHitInfo);

  std::string particle_info_table_name = "particles";
  _memtypeParticleInfo = createParticleInfoType();
  _particleInfoTable = createTable(_group, particle_info_table_name, _memtypeParticleInfo);

  std::string sns_pos_table_name = "sns_positions";
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

void HDF5Writer::WriteSensorDataInfo(int evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_data_t snsData;
  snsData.event_id = evt_number;
  snsData.sensor_id = sensor_id;
  snsData.time_bin = time_bin;
  snsData.charge = charge;
  writeSnsData(&snsData, _snsDataTable, _memtypeSnsData, _ismp);

  _ismp++;
}

void HDF5Writer::WriteSensorTofInfo(int evt_number, int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_tof_t snsTof;
  snsTof.event_id = evt_number;
  snsTof.sensor_id = sensor_id;
  snsTof.time_bin = time_bin;
  snsTof.charge = charge;
  writeSnsTof(&snsTof, _snsTofTable, _memtypeSnsTof, _ismp_tof);

  _ismp_tof++;
}

void HDF5Writer::WriteHitInfo(int evt_number, int particle_indx, float hit_position_x, float hit_position_y, float hit_position_z, float hit_time, float hit_energy, const char* label)
{
  hit_info_t trueInfo;
  trueInfo.event_id = evt_number;
  memset(trueInfo.label, 0, STRLEN);
  trueInfo.x = hit_position_x;
  trueInfo.y = hit_position_y;
  trueInfo.z = hit_position_z;
  trueInfo.time = hit_time;
  trueInfo.energy = hit_energy;
  strcpy(trueInfo.label, label);
  trueInfo.particle_id = particle_indx;
  writeHit(&trueInfo,  _hitInfoTable, _memtypeHitInfo, _ihit);

  _ihit++;
}

void HDF5Writer::WriteParticleInfo(int evt_number, int particle_indx, const char* particle_name, char primary, int mother_id, float initial_vertex_x, float initial_vertex_y, float initial_vertex_z, float initial_vertex_t, float final_vertex_x, float final_vertex_y, float final_vertex_z, float final_vertex_t, const char* initial_volume, const char* final_volume, float momentum_x, float momentum_y, float momentum_z, float final_momentum_x, float final_momentum_y, float final_momentum_z, float kin_energy, float length, const char* creator_proc, const char* final_proc)
{
  particle_info_t trueInfo;
  trueInfo.event_id = evt_number;
  trueInfo.particle_id = particle_indx;
  memset(trueInfo.particle_name, 0, STRLEN);
  strcpy(trueInfo.particle_name, particle_name);
  trueInfo.primary = primary;
  trueInfo.mother_id = mother_id;
  trueInfo.initial_x = initial_vertex_x;
  trueInfo.initial_y = initial_vertex_y;
  trueInfo.initial_z = initial_vertex_z;
  trueInfo.initial_t = initial_vertex_t;
  trueInfo.final_x = final_vertex_x;
  trueInfo.final_y = final_vertex_y;
  trueInfo.final_z = final_vertex_z;
  trueInfo.final_t = final_vertex_t;
  memset(trueInfo.initial_volume, 0, STRLEN);
  strcpy(trueInfo.initial_volume, initial_volume);
  memset(trueInfo.final_volume, 0, STRLEN);
  strcpy(trueInfo.final_volume, final_volume);
  trueInfo.initial_momentum_x = momentum_x;
  trueInfo.initial_momentum_y = momentum_y;
  trueInfo.initial_momentum_z = momentum_z;
  trueInfo.final_momentum_x = final_momentum_x;
  trueInfo.final_momentum_y = final_momentum_y;
  trueInfo.final_momentum_z = final_momentum_z;
  trueInfo.kin_energy = kin_energy;
  trueInfo.length = length;
  memset(trueInfo.creator_proc, 0, STRLEN);
  strcpy(trueInfo.creator_proc, creator_proc);
  memset(trueInfo.final_proc, 0, STRLEN);
  strcpy(trueInfo.final_proc, final_proc);
  writeParticle(&trueInfo,  _particleInfoTable, _memtypeParticleInfo, _ipart);

  _ipart++;
}

void HDF5Writer::WriteSensorPosInfo(unsigned int sensor_id, const char* sensor_name, float x, float y, float z)
{
  sns_pos_t snsPos;
  snsPos.sensor_id = sensor_id;
  memset(snsPos.sensor_name, 0, STRLEN);
  strcpy(snsPos.sensor_name, sensor_name);
  snsPos.x = x;
  snsPos.y = y;
  snsPos.z = z;
  writeSnsPos(&snsPos, _snsPosTable, _memtypeSnsPos, _ipos);

  _ipos++;
}
