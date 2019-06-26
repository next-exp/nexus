#include "hdf5_functions.h"

hsize_t createRunType()
{
  hid_t strtype = H5Tcopy(H5T_C_S1);
  H5Tset_size (strtype, CONFLEN);

  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (run_info_t));
  H5Tinsert (memtype, "param_key" , HOFFSET (run_info_t, param_key), strtype);
  H5Tinsert (memtype, "param_value" , HOFFSET (run_info_t, param_value), strtype);
  return memtype;
}

hsize_t createSensorDataType()
{
  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (sns_data_t));
  H5Tinsert (memtype, "event_id", HOFFSET (sns_data_t, event_id), H5T_NATIVE_INT32);
  H5Tinsert (memtype, "sensor_id" , HOFFSET (sns_data_t, sensor_id) , H5T_NATIVE_UINT);
  H5Tinsert (memtype, "time_bin" , HOFFSET (sns_data_t, time_bin) , H5T_NATIVE_UINT64);
  H5Tinsert (memtype, "charge" , HOFFSET (sns_data_t, charge) , H5T_NATIVE_UINT);
  return memtype;
}

hsize_t createSensorTofType()
{
  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (sns_tof_t));
  H5Tinsert (memtype, "event_id", HOFFSET (sns_tof_t, event_id), H5T_NATIVE_INT32);
  H5Tinsert (memtype, "sensor_id" , HOFFSET (sns_tof_t, sensor_id) , H5T_NATIVE_INT);
  H5Tinsert (memtype, "time_bin" , HOFFSET (sns_tof_t, time_bin) , H5T_NATIVE_UINT);
  H5Tinsert (memtype, "charge" , HOFFSET (sns_tof_t, charge) , H5T_NATIVE_UINT);
  return memtype;
}

hsize_t createHitInfoType()
{
  // hsize_t point3d_dim[1] = {3};
  // hid_t point3d = H5Tarray_create(H5T_NATIVE_FLOAT, 1, point3d_dim);

  hid_t strtype = H5Tcopy(H5T_C_S1);
  H5Tset_size (strtype, STRLEN);

  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (hit_info_t));
  H5Tinsert (memtype, "event_id", HOFFSET (hit_info_t, event_id), H5T_NATIVE_INT32);
  H5Tinsert (memtype, "x",HOFFSET (hit_info_t, x), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "y",HOFFSET (hit_info_t, y), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "z",HOFFSET (hit_info_t, z), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "time",HOFFSET (hit_info_t, time), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "energy",HOFFSET (hit_info_t, energy), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "label",HOFFSET (hit_info_t, label),strtype);
  H5Tinsert (memtype, "particle_id",HOFFSET (hit_info_t, particle_id),H5T_NATIVE_INT);
  H5Tinsert (memtype, "hit_id",HOFFSET (hit_info_t, hit_id), H5T_NATIVE_INT);


void writeSnsPos(sns_pos_t* snsPos, hid_t dataset, hid_t memtype, hsize_t counter)
{
  hid_t memspace, file_space;
  //Create memspace for one more row
  const hsize_t n_dims = 1;
  hsize_t dims[n_dims] = {1};
  memspace = H5Screate_simple(n_dims, dims, NULL);

  //Extend dataset
  dims[0] = counter+1;
  H5Dset_extent(dataset, dims);

  //Write waveforms
  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {counter};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, snsPos);
  H5Sclose(file_space);
  H5Sclose(memspace);
}
