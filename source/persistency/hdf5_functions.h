#ifndef HDF5_FUNCTIONS_H
#define HDF5_FUNCTIONS_H

#include <hdf5.h>
#include <iostream>

   typedef struct{ 
     unsigned int simulated_events;
     unsigned int saved_events;
     char param_key[100];
     char param_value[100];
   } run_info_t; 

  typedef struct{
    int evt_number;
    unsigned int init_sns_data;
    unsigned int init_hit;
    unsigned int init_particle;
  } evt_extent_t;

  typedef struct{
    int evt_number;
    float evt_energy;
  } evt_t;

  typedef struct{
    unsigned int sensor_id;
    unsigned int time_bin;
    unsigned int charge;
  } sns_data_t;

  typedef struct{
	int hit_indx;
	float hit_position[3];
	float hit_time;
	float hit_energy;
        char label[20];
        int track_indx;
  } hit_info_t;

  typedef struct{
	int track_indx;
	char particle_name[20];
        char primary;
	float initial_vertex[4];
	float final_vertex[4];
        char initial_volume[20];
        char final_volume[20];
	float momentum[3];
	float kin_energy;
        char creator_proc[20];
  } particle_info_t;

  hsize_t createRunType();
  hsize_t createEventType();
  hsize_t createSensorDataType();
  hsize_t createHitInfoType();
  hsize_t createParticleInfoType();
  hsize_t createEventExtentType();

  hid_t createTable(hid_t group, std::string& table_name, hsize_t memtype);
  hid_t createGroup(hid_t file, std::string& groupName);

  void writeRun(run_info_t* runData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeEvent(evt_t* evtData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeSnsData(sns_data_t* snsData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeHit(hit_info_t* hitInfo, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeParticle(particle_info_t* particleInfo, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeEventExtent(evt_extent_t* evtExtent, hid_t dataset, hid_t memtype, hsize_t counter);


#endif
