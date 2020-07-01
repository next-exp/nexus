// ----------------------------------------------------------------------------
// nexus | HDF5Writer.cc
//
// This class writes the h5 nexus output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HDF5Writer.h"

#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <stdint.h>
#include <iostream>

using namespace nexus;


HDF5Writer::HDF5Writer():
  file_(0), irun_(0), ismp_(0),
  ismp_tof_(0), ihit_(0),
  ipart_(0), ipos_(0)
{
}

HDF5Writer::~HDF5Writer()
{
}

void HDF5Writer::Open(std::string fileName)
{
  firstEvent_= true;

  file_ = H5Fcreate( fileName.c_str(), H5F_ACC_TRUNC,
                      H5P_DEFAULT, H5P_DEFAULT );

  std::string group_name = "/MC";
  group_ = createGroup(file_, group_name);

  std::string run_table_name = "configuration";
  memtypeRun_ = createRunType();
  runTable_ = createTable(group_, run_table_name, memtypeRun_);

  std::string sns_data_table_name = "sns_response";
  memtypeSnsData_ = createSensorDataType();
  snsDataTable_ = createTable(group_, sns_data_table_name, memtypeSnsData_);

  std::string sns_tof_table_name = "tof_sns_response";
  memtypeSnsTof_ = createSensorTofType();
  snsTofTable_ = createTable(group_, sns_tof_table_name, memtypeSnsTof_);

  std::string hit_info_table_name = "hits";
  memtypeHitInfo_ = createHitInfoType();
  hitInfoTable_ = createTable(group_, hit_info_table_name, memtypeHitInfo_);

  std::string particle_info_table_name = "particles";
  memtypeParticleInfo_ = createParticleInfoType();
  particleInfoTable_ = createTable(group_, particle_info_table_name, memtypeParticleInfo_);

  std::string sns_pos_table_name = "sns_positions";
  memtypeSnsPos_ = createSensorPosType();
  snsPosTable_ = createTable(group_, sns_pos_table_name, memtypeSnsPos_);

  isOpen_ = true;
}

void HDF5Writer::Close()
{
  isOpen_=false;
  H5Fclose(file_);
}

void HDF5Writer::WriteRunInfo(const char* param_key, const char* param_value)
{
  run_info_t runData;
  memset(runData.param_key,   0, CONFLEN);
  memset(runData.param_value, 0, CONFLEN);
  strcpy(runData.param_key, param_key);
  strcpy(runData.param_value, param_value);
  writeRun(&runData, runTable_, memtypeRun_, irun_);

  irun_++;
}

void HDF5Writer::WriteSensorDataInfo(int evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_data_t snsData;
  snsData.event_id = evt_number;
  snsData.sensor_id = sensor_id;
  snsData.time_bin = time_bin;
  snsData.charge = charge;
  writeSnsData(&snsData, snsDataTable_, memtypeSnsData_, ismp_);

  ismp_++;
}

void HDF5Writer::WriteSensorTofInfo(int evt_number, int sensor_id, unsigned int time_bin, unsigned int charge)
{
  sns_tof_t snsTof;
  snsTof.event_id = evt_number;
  snsTof.sensor_id = sensor_id;
  snsTof.time_bin = time_bin;
  snsTof.charge = charge;
  writeSnsTof(&snsTof, snsTofTable_, memtypeSnsTof_, ismp_tof_);

  ismp_tof_++;
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
  writeHit(&trueInfo,  hitInfoTable_, memtypeHitInfo_, ihit_);

  ihit_++;
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
  writeParticle(&trueInfo,  particleInfoTable_, memtypeParticleInfo_, ipart_);

  ipart_++;
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
  writeSnsPos(&snsPos, snsPosTable_, memtypeSnsPos_, ipos_);

  ipos_++;
}
