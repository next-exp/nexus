#include "hdf5_functions.h"

hsize_t createSensorDataType()
{
  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (sns_data_t));
  H5Tinsert (memtype, "sensor_id" , HOFFSET (sns_data_t, sensor_id) , H5T_NATIVE_UINT);
  H5Tinsert (memtype, "time_bin" , HOFFSET (sns_data_t, time_bin) , H5T_NATIVE_UINT);
  H5Tinsert (memtype, "charge" , HOFFSET (sns_data_t, charge) , H5T_NATIVE_UINT);
  return memtype;
}

hsize_t createEventType()
{
  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (evt_t));
  H5Tinsert (memtype, "evt_number", HOFFSET (evt_t, evt_number), H5T_NATIVE_INT);
  //H5Tinsert (memtype, "timestamp" , HOFFSET (evt_t, timestamp) , H5T_NATIVE_UINT64);
  return memtype;
}

hid_t createRunType()
{
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (runinfo_t));
  H5Tinsert (memtype, "run_number", HOFFSET (runinfo_t, run_number), H5T_NATIVE_INT);
  return memtype;
}

hid_t createSensorType()
{
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (sensor_t));
  H5Tinsert (memtype, "channel",  HOFFSET(sensor_t, channel) , H5T_NATIVE_INT);
  H5Tinsert (memtype, "sensorID", HOFFSET(sensor_t, sensorID), H5T_NATIVE_INT);
  return memtype;
}

hid_t createTable(hid_t group, std::string& table_name, hsize_t memtype)
{
  //Create 1D dataspace (evt number). First dimension is unlimited (initially 0)
  const hsize_t ndims = 1;
  hsize_t dims[ndims] = {0};
  hsize_t max_dims[ndims] = {H5S_UNLIMITED};
  hsize_t file_space = H5Screate_simple(ndims, dims, max_dims);

  // Create a dataset creation property list
  // The layout of the dataset have to be chunked when using unlimited dimensions
  hid_t plist = H5Pcreate(H5P_DATASET_CREATE);
  H5Pset_layout(plist, H5D_CHUNKED);
  hsize_t chunk_dims[ndims] = {32768};
  H5Pset_chunk(plist, ndims, chunk_dims);

  // Create dataset
  hid_t dataset = H5Dcreate(group, table_name.c_str(), memtype, file_space,
                            H5P_DEFAULT, plist, H5P_DEFAULT);
  return dataset;
}

hid_t createGroup(hid_t file, std::string& groupName)
{
  //Create group
  hid_t wfgroup;
  wfgroup = H5Gcreate2(file, groupName.c_str(), H5P_DEFAULT, H5P_DEFAULT,
                       H5P_DEFAULT);
  return wfgroup;
}

void writeSnsData(sns_data_t* snsData, hid_t dataset, hid_t memtype, hsize_t counter)
{
  hid_t memspace, file_space;
  //Create memspace for one more SiPM row
  const hsize_t n_dims = 1;
  hsize_t dims[n_dims] = {1};
  memspace = H5Screate_simple(n_dims, dims, NULL);

  //Extend SiPM dataset
  dims[0] = counter+1;
  H5Dset_extent(dataset, dims);

  //Write SiPM waveforms
  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {counter};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, snsData);
  H5Sclose(file_space);
}

void writeEvent(evt_t* evtData, hid_t dataset, hid_t memtype, hsize_t counter)
{
  hid_t memspace, file_space;
  hsize_t dims[1] = {1};
  memspace = H5Screate_simple(1, dims, NULL);

  //Extend dataset
  dims[0] = counter+1;
  H5Dset_extent(dataset, dims);

  //Write event info
  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {counter};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, evtData);
  H5Sclose(file_space);
}

void writeRun(runinfo_t * runData, hid_t dataset, hid_t memtype, hsize_t evt_number)
{
  hid_t memspace, file_space;
  hsize_t dims[1] = {1};
  memspace = H5Screate_simple(1, dims, NULL);

  //Extend SiPM dataset
  dims[0] = evt_number+1;
  H5Dset_extent(dataset, dims);

  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {evt_number};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, runData);
  H5Sclose(file_space);
}


