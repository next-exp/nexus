#ifndef HDF5WRITER_H
#define HDF5WRITER_H

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

    void Write(std::uint64_t timestamp,
               unsigned int evt_number, size_t run_number);

  private:
    size_t _file; ///< HDF5 file

    bool _isOpen;     
    bool _firstEvent; ///< First event

    size_t _rinfoG; ///< group for run info
    
    //Datasets
    size_t _extpmtrd;
    size_t _eventsTable;
    size_t _sipmrd;
    size_t _memtypeEvt;

    size_t _ievt; ///< counter for written events
    
  };
  
} // namespace nexus

#endif
