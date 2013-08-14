//
//  DetectorConstruction.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created:  9 Mar 2009
//     Updated: 14 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __DETECTOR_CONSTRUCTION__
#define __DETECTOR_CONSTRUCTION__

#include <G4VUserDetectorConstruction.hh>


namespace nexus {

  class BaseGeometry;
  
  class DetectorConstruction: public G4VUserDetectorConstruction
  {
  public:
    /// Constructor
    DetectorConstruction();
    /// Destructor
    ~DetectorConstruction();

    /// Returns the physical volume that represents the world.
    /// It is the Geant4 mandatory method invoked by G4RunManager.
    G4VPhysicalVolume* Construct();

    ///
    void SetGeometry(BaseGeometry*);

  private:
    void AssertGeometry();

  private:
    BaseGeometry* _geometry;
    G4bool _init;
  };

} // namespace nexus

#endif
