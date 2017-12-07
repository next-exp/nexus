#ifndef HDF5_FUNCTIONS_H
#define HDF5_FUNCTIONS_H

#include <hdf5.h>
#include <iostream>

  typedef struct{
    int channel;
    int sensorID;
  } sensor_t;

  typedef struct{
    int run_number;
  } runinfo_t;

  typedef struct{
    int evt_number;
  } evt_t;

  typedef struct{
    int sensor_id;
    uint64_t time_bin;
    float charge;
  } sns_data_t;

  hsize_t createSensorDataType();
  hsize_t createEventType();
  hid_t createRunType();
  hid_t createSensorType();
  hid_t createTable(hid_t group, std::string& table_name, hsize_t memtype);
  hid_t createGroup(hid_t file, std::string& groupName);

  //  void WriteWaveforms(short int * data, hid_t dataset, hsize_t nsensors, hsize_t nsamples, hsize_t evt);
  // hid_t createWaveforms(hid_t group, std::string& dataset, hsize_t nsensors, hsize_t nsamples);

  // void WriteWaveform(short int * data, hid_t dataset, hsize_t nsamples, hsize_t evt);
  // hid_t createWaveform(hid_t group, std::string& dataset, hsize_t nsamples);

  void writeSnsData(sns_data_t* snsData, hid_t dataset, hid_t memtype, hsize_t counter);
  void writeEvent(evt_t * evtData, hid_t dataset, hid_t memtype, hsize_t evt_number);
  void writeRun(runinfo_t * runData, hid_t dataset, hid_t memtype, hsize_t evt_number);
  // void writeSensor(sensor_t * sensorData, hid_t dataset, hid_t memtype, hsize_t sensor_number);
 


#endif

