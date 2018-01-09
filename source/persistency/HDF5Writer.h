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

    void WriteRunInfo(const char* param_key, const char* param_value);
    void WriteEventInfo(int evt_number, float evt_energy);
    void WriteSensorDataInfo(unsigned int sensor_id, unsigned int time_bin, unsigned int charge);
    void WriteHitInfo(int track_indx, int hit_indx, const float* hit_position, int size_position, float hit_time, float hit_energy, const char* label);
    void WriteParticleInfo(int track_indx, const char* particle_name, char primary, int mother_id, const float* initial_vertex, int size_initial_vertex, const float* final_vertex, int size_final_vertex, const char* initial_volume, const char* final_volume, const float* momentum, int size_momentum, float kin_energy, const char* creator_proc);
    void WriteEventExtentInfo(int evt_number, unsigned int last_sns_data, unsigned int last_hit, unsigned int last_particle);

    size_t GetSnsDataIndex() const;
    size_t GetHitIndex() const;
    size_t GetParticleIndex() const;
    
  private:
    size_t _file; ///< HDF5 file

    bool _isOpen;     
    bool _firstEvent; ///< First event

    size_t _group; ///< group for everything
    
    //Datasets
    size_t _runTable;
    size_t _eventsTable;
    size_t _snsDataTable;
    size_t _hitInfoTable;
    size_t _particleInfoTable;
    size_t _evtExtentTable;

    size_t _memtypeRun;
    size_t _memtypeEvt;
    size_t _memtypeSnsData;
    size_t _memtypeHitInfo;
    size_t _memtypeParticleInfo;
    size_t _memtypeEventExtent;

    size_t _irun; ///< counter for configuration parameters
    size_t _ievt; ///< counter for written events
    size_t _ismp; ///< counter for written waveform samples
    size_t _ihit; ///< counter for true information
    size_t _ipart; ///< counter for particle information
    size_t _ievt_extent; ///< counter for extent info
    
  };

  inline size_t HDF5Writer::GetSnsDataIndex() const {return _ismp -1;}
  inline size_t HDF5Writer::GetHitIndex() const {return _ihit -1;}
  inline size_t HDF5Writer::GetParticleIndex() const {return _ipart -1;}
  
} // namespace nexus

#endif
