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

    void WriteEventInfo(unsigned int evt_number);

    void WriteRunInfo(size_t run_number);
    void WriteSensorDataInfo(unsigned int sensor_id, unsigned int time_bin, unsigned int charge);

  private:
    size_t _file; ///< HDF5 file

    bool _isOpen;     
    bool _firstEvent; ///< First event

    size_t _group; ///< group for everything
    //size_t _group_truth; ///< group for MC truth info
    size_t _rinfoG; ///< group for run info
    
    //Datasets
    size_t _extpmtrd;
    size_t _eventsTable;
    size_t _sipmrd;
    size_t _memtypeEvt;
    
    size_t _memtypeSnsData;
    size_t _snsDataTable;

    size_t _ievt; ///< counter for written events
    size_t _ismp; ///< counter for written waveform samples
    
  };
  
} // namespace nexus

#endif
