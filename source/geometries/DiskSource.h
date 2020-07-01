// ----------------------------------------------------------------------------
// nexus | DiskSource.h
//
// Calibration source with disk shape, used for the bigger source at LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

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
    G4double source_diam_;
    G4double source_thick_;

    // Dimension of the whole support
    G4double support_diam_;
    G4double support_thick_;
    

  };

  inline G4double DiskSource::GetSourceDiameter() const { return source_diam_;}

  inline G4double DiskSource::GetSourceThickness() const { return source_thick_;}

  inline G4double DiskSource::GetSupportDiameter() const { return support_diam_;}

  inline G4double DiskSource::GetSupportThickness() const { return support_thick_;}
  
}
#endif
