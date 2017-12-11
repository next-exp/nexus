#ifndef HDF5WRITER_H
#define HDF5WRITER_H

#include "hdf5_functions.h"

#include <hdf5.h>
#include <iostream>

namespace nexus {

  class HDF5Writer {

  public:
    //! constructor
    HDF5Writer();
    /// destructor
    ~HDF5Writer();

    //! open file
    void Open(std::string filename);

    //! close file
    void Close();

    void WriteEventInfo(unsigned int evt_number, float evt_energy);
    void WriteSensorDataInfo(unsigned int sensor_id, unsigned int time_bin, unsigned int charge);
    void WriteHitInfo(int track_indx, int hit_indx, const float* hit_position, float hit_time, float hit_energy, const char* label);
    void WriteParticleInfo(int track_indx, const char* particle_name, char primary, int pdg_code, const float* initial_vertex, const float* final_vertex, const char* initial_volume, const char* final_volume, const float* momentum, float energy);
    //    void WriteRunInfo(size_t run_number);
    
  private:
    size_t _file; ///< HDF5 file

    bool _isOpen;     
    bool _firstEvent; ///< First event

    size_t _group; ///< group for everything
    
    //Datasets
    size_t _eventsTable;
    size_t _snsDataTable;
    size_t _hitInfoTable;
    size_t _particleInfoTable;
    
    size_t _memtypeEvt;
    size_t _memtypeSnsData;
    size_t _memtypeHitInfo;
    size_t _memtypeParticleInfo;

    size_t _ievt; ///< counter for written events
    size_t _ismp; ///< counter for written waveform samples
    size_t _ihit; ///< counter for true information
    size_t _ipart; ///< counter for particle information
    
  };
  
} // namespace nexus

#endif
