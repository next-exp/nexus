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
  return memtype;
}

hsize_t createParticleInfoType()
{
  // hsize_t point3d_dim[1] = {3};
  // hid_t point3d = H5Tarray_create(H5T_NATIVE_FLOAT, 1, point3d_dim);

  // hsize_t point_dim[1] = {4};
  // hid_t point = H5Tarray_create(H5T_NATIVE_FLOAT, 1, point_dim);

  hid_t strtype = H5Tcopy(H5T_C_S1);
  H5Tset_size (strtype, STRLEN);

  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (particle_info_t));
  H5Tinsert (memtype, "event_id", HOFFSET (particle_info_t, event_id), H5T_NATIVE_INT32);
  H5Tinsert (memtype, "particle_id",HOFFSET (particle_info_t, particle_id),H5T_NATIVE_INT);
  H5Tinsert (memtype, "particle_name",HOFFSET (particle_info_t, particle_name),strtype);
  H5Tinsert (memtype, "primary",HOFFSET (particle_info_t, primary), H5T_NATIVE_CHAR);
  H5Tinsert (memtype, "mother_id",HOFFSET (particle_info_t, mother_id),H5T_NATIVE_INT);
  H5Tinsert (memtype, "initial_x",HOFFSET (particle_info_t, initial_x), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "initial_y",HOFFSET (particle_info_t, initial_y), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "initial_z",HOFFSET (particle_info_t, initial_z), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "initial_t",HOFFSET (particle_info_t, initial_t), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_x",HOFFSET (particle_info_t, final_x), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_y",HOFFSET (particle_info_t, final_y), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_z",HOFFSET (particle_info_t, final_z), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_t",HOFFSET (particle_info_t, final_t), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "initial_volume",HOFFSET (particle_info_t, initial_volume),strtype);
  H5Tinsert (memtype, "final_volume",HOFFSET (particle_info_t, final_volume),strtype);
  H5Tinsert (memtype, "initial_momentum_x",HOFFSET (particle_info_t, initial_momentum_x), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "initial_momentum_y",HOFFSET (particle_info_t, initial_momentum_y), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "initial_momentum_z",HOFFSET (particle_info_t, initial_momentum_z), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_momentum_x", HOFFSET (particle_info_t, final_momentum_x), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_momentum_y", HOFFSET (particle_info_t, final_momentum_y), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "final_momentum_z", HOFFSET (particle_info_t, final_momentum_z), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "kin_energy",HOFFSET (particle_info_t, kin_energy), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "length", HOFFSET (particle_info_t, length), H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "creator_proc",HOFFSET (particle_info_t, creator_proc), strtype);
  H5Tinsert (memtype, "final_proc", HOFFSET (particle_info_t, final_proc), strtype);
  return memtype;
}

hsize_t createSensorPosType()
{
  hid_t strtype = H5Tcopy(H5T_C_S1);
  H5Tset_size (strtype, STRLEN);

  //Create compound datatype for the table
  hsize_t memtype = H5Tcreate (H5T_COMPOUND, sizeof (sns_pos_t));
  H5Tinsert (memtype, "sensor_id" , HOFFSET (sns_pos_t, sensor_id) , H5T_NATIVE_UINT);
  H5Tinsert (memtype, "sensor_name", HOFFSET (sns_pos_t, sensor_name), strtype);
  H5Tinsert (memtype, "x" , HOFFSET (sns_pos_t, x) , H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "y" , HOFFSET (sns_pos_t, y) , H5T_NATIVE_FLOAT);
  H5Tinsert (memtype, "z" , HOFFSET (sns_pos_t, z) , H5T_NATIVE_FLOAT);
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

  //Set compression
  //  int clevel = 0;
  //  H5Pset_deflate (plist, clevel);

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

void writeRun(run_info_t* runData, hid_t dataset, hid_t memtype, hsize_t counter)
{
  hid_t memspace, file_space;
  hsize_t dims[1] = {1};
  memspace = H5Screate_simple(1, dims, NULL);

  //Extend dataset
  dims[0] = counter+1;
  H5Dset_extent(dataset, dims);

  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {counter};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, runData);
  H5Sclose(file_space);
  H5Sclose(memspace);
}

void writeSnsData(sns_data_t* snsData, hid_t dataset, hid_t memtype, hsize_t counter)
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
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, snsData);
  H5Sclose(file_space);
  H5Sclose(memspace);
}

void writeSnsTof(sns_tof_t* snsTof, hid_t dataset, hid_t memtype, hsize_t counter)
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
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, snsTof);
  H5Sclose(file_space);
  H5Sclose(memspace);
}


void writeHit(hit_info_t* hitInfo, hid_t dataset, hid_t memtype, hsize_t counter)
{
  hid_t memspace, file_space;

  const hsize_t n_dims = 1;
  hsize_t dims[n_dims] = {1};
  memspace = H5Screate_simple(n_dims, dims, NULL);

  dims[0] = counter+1;
  H5Dset_extent(dataset, dims);

  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {counter};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, hitInfo);
  H5Sclose(file_space);
  H5Sclose(memspace);
}

void writeParticle(particle_info_t* particleInfo, hid_t dataset, hid_t memtype, hsize_t counter)
{
  hid_t memspace, file_space;

  const hsize_t n_dims = 1;
  hsize_t dims[n_dims] = {1};
  memspace = H5Screate_simple(n_dims, dims, NULL);

  dims[0] = counter+1;
  H5Dset_extent(dataset, dims);

  file_space = H5Dget_space(dataset);
  hsize_t start[1] = {counter};
  hsize_t count[1] = {1};
  H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count, NULL);
  H5Dwrite(dataset, memtype, memspace, file_space, H5P_DEFAULT, particleInfo);
  H5Sclose(file_space);
  H5Sclose(memspace);
}


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
