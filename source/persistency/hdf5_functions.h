#ifndef HDF5_FUNCTIONS_H
#define HDF5_FUNCTIONS_H

#include <hdf5.h>
#include <iostream>

#define CONFLEN 100
#define STRLEN 20

  typedef struct{
     char param_key[CONFLEN];
     char param_value[CONFLEN];
   } run_info_t; 

  typedef struct{
    int32_t evt_number;
    uint64_t last_sns_data;
    uint64_t last_sns_tof;
    uint64_t last_hit;
    uint64_t last_particle;
  } evt_extent_t;

  typedef struct{
    int32_t evt_number;
    float evt_energy;
  } evt_t;

  typedef struct{
    unsigned int sensor_id;
    uint64_t time_bin;
    unsigned int charge;
  } sns_data_t;

  typedef struct{
    int sensor_id;
    unsigned int time_bin;
    unsigned int charge;
  } sns_tof_t;

  typedef struct{
	float hit_position[3];
	float hit_time;
	float hit_energy;
        char label[STRLEN];
        int particle_indx;
        int hit_indx;
  } hit_info_t;

  typedef struct{
	int particle_indx;
	char particle_name[STRLEN];
        char primary;
	int mother_indx;
	float initial_vertex[4];
	float final_vertex[4];
        char initial_volume[STRLEN];
        char final_volume[STRLEN];
	float momentum[3];
	float kin_energy;
        char creator_proc[STRLEN];
  } particle_info_t;

  typedef struct{
    unsigned int sensor_id;
    float x;
    float y;
    float z;
  } sns_pos_t;

  hsize_t createRunType();
  hsize_t createEventType();
  hsize_t createSensorDataType();
  hsize_t createSensorTofType();
  hsize_t createHitInfoType();
  hsize_t createParticleInfoType();
  hsize_t createEventExtentType();
  hsize_t createSensorPosType();

  hid_t createTable(hid_t group, std::string& table_name, hsize_t memtype);
  hid_t createGroup(hid_t file, std::string& groupName);

  void writeRun(run_info_t* runData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeEvent(evt_t* evtData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsData(sns_data_t* snsData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsTof(sns_tof_t* snsTof, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeHit(hit_info_t* hitInfo, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeParticle(particle_info_t* particleInfo, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeEventExtent(evt_extent_t* evtExtent, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsPos(sns_pos_t* snsPos, hid_t dataset, hid_t memtype, hsize_t counter);


#endif

