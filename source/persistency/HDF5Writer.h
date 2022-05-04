// ----------------------------------------------------------------------------
// nexus | HDF5Writer.cc
//
// This class writes the h5 nexus output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HDF5WRITER_H
#define HDF5WRITER_H

#include "hdf5_functions.h"

#include <hdf5.h>
#include <iostream>

namespace nexus {

  class HDF5Writer {

  public:
    /// constructor
    HDF5Writer();
    /// destructor
    ~HDF5Writer();

    /// open file
    void Open(std::string filename, bool debug);

    /// close file
    void Close();

    void WriteRunInfo(const char* param_key, const char* param_value);
    void WriteSensorDataInfo(int64_t evt_number, unsigned int sensor_id, unsigned int time_bin, unsigned int charge);
    void WriteHitInfo(int64_t evt_number, int particle_indx, int hit_indx, float hit_position_x, float hit_position_y, float hit_position_z, float hit_time, float hit_energy, const char* label);
    void WriteParticleInfo(int64_t evt_number, int particle_indx, const char* particle_name, char primary, int mother_id, float initial_vertex_x, float initial_vertex_y, float initial_vertex_z, float initial_vertex_t, float final_vertex_x, float final_vertex_y, float final_vertex_z, float final_vertex_t, const char* initial_volume, const char* final_volume, float ini_momentum_x, float ini_momentum_y, float ini_momentum_z, float final_momentum_x, float final_momentum_y, float final_momentum_z, float kin_energy, float length, const char* creator_proc, const char* final_proc);
    void WriteSensorPosInfo(unsigned int sensor_id, const char* sensor_name, float x, float y, float z);
    void WriteStep(int64_t evt_number,
                   int particle_id, const char* particle_name,
                   int step_id,
                   const char* initial_volume,
                   const char*   final_volume,
                   const char*      proc_name,
                   float initial_x, float initial_y, float initial_z,
                   float   final_x, float   final_y, float   final_z);

  private:
    size_t file_; ///< HDF5 file

    bool isOpen_;
    bool firstEvent_; ///< First event

    //Datasets
    size_t runTable_;
    size_t snsDataTable_;
    size_t hitInfoTable_;
    size_t particleInfoTable_;
    size_t snsPosTable_;
    size_t stepTable_;

    size_t memtypeRun_;
    size_t memtypeSnsData_;
    size_t memtypeHitInfo_;
    size_t memtypeParticleInfo_;
    size_t memtypeSnsPos_;
    size_t memtypeStep_;

    size_t irun_; ///< counter for configuration parameters
    size_t ismp_; ///< counter for written waveform samples
    size_t ihit_; ///< counter for true information
    size_t ipart_; ///< counter for particle information
    size_t ipos_; ///< counter for sensor positions
    size_t istep_; ///< counter for steps

  };

} // namespace nexus

#endif
