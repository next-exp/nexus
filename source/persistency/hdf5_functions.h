// ----------------------------------------------------------------------------
// nexus | hdf5_functions.h
//
// Collection of functions to create h5 tables for nexus output.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HDF5_FUNCTIONS_H
#define HDF5_FUNCTIONS_H

#include <hdf5.h>
#include <iostream>

#define CONFLEN 300
#define STRLEN 100

  typedef struct{
     char param_key[CONFLEN];
     char param_value[CONFLEN];
   } run_info_t;

  typedef struct{
    int32_t event_id;
    unsigned int sensor_id;
    uint64_t time_bin;
    unsigned int charge;
  } sns_data_t;

  typedef struct{
    int32_t event_id;
    int sensor_id;
    unsigned int time_bin;
    unsigned int charge;
  } sns_tof_t;

  typedef struct{
    int32_t event_id;
    float x;
    float y;
    float z;
    float time;
    float energy;
    char label[STRLEN];
    int particle_id;
  } hit_info_t;

typedef struct{
    int32_t event_id;
    int particle_id;
    char particle_name[STRLEN];
    char primary;
    int mother_id;
    float initial_x;
    float initial_y;
    float initial_z;
    float initial_t;
    float final_x;
    float final_y;
    float final_z;
    float final_t;
    char initial_volume[STRLEN];
    char final_volume[STRLEN];
    float initial_momentum_x;
    float initial_momentum_y;
    float initial_momentum_z;
    float final_momentum_x;
    float final_momentum_y;
    float final_momentum_z;
    float kin_energy;
    float length;
    char creator_proc[STRLEN];
    char final_proc[STRLEN];
  } particle_info_t;

  typedef struct{
    unsigned int sensor_id;
    char sensor_name[STRLEN];
    float x;
    float y;
    float z;
  } sns_pos_t;

  hsize_t createRunType();
  hsize_t createSensorDataType();
  hsize_t createSensorTofType();
  hsize_t createHitInfoType();
  hsize_t createParticleInfoType();
  hsize_t createSensorPosType();

  hid_t createTable(hid_t group, std::string& table_name, hsize_t memtype);
  hid_t createGroup(hid_t file, std::string& groupName);

  void writeRun(run_info_t* runData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsData(sns_data_t* snsData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsTof(sns_tof_t* snsTof, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeHit(hit_info_t* hitInfo, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeParticle(particle_info_t* particleInfo, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsPos(sns_pos_t* snsPos, hid_t dataset, hid_t memtype, hsize_t counter);


#endif
