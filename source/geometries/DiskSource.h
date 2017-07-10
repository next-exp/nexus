#ifndef DISK_SOURCE_H
#define DISK_SOURCE_H

#include "BaseGeometry.h"

namespace nexus {

  class DiskSource: public BaseGeometry {
  public:
    /// Constructor
    DiskSource() {}

    /// Destructor
    ~DiskSource() {}

    //  void Construct();
   
    G4double GetSourceDiameter() const; 
    G4double GetSourceThickness() const;
    
    G4double GetSupportDiameter() const;
    G4double GetSupportThickness() const;

  protected:
    
    // Dimension of the source itself
    G4double _source_diam;
    G4double _source_thick;

    // Dimension of the whole support
    G4double _support_diam;
    G4double _support_thick;
    

  };

  inline G4double DiskSource::GetSourceDiameter() const { return _source_diam;}

  inline G4double DiskSource::GetSourceThickness() const { return _source_thick;}

  inline G4double DiskSource::GetSupportDiameter() const { return _support_diam;}

  inline G4double DiskSource::GetSupportThickness() const { return _support_thick;}
  
}
#endif
