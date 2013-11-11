// ----------------------------------------------------------------------------
///  \file   DetectorConstruction.h
///  \brief  Initialization class for detector setup.
///  
///  \author   <justo.martin-albo@ific.uv.es>    
///  \date     9 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>

class G4GenericMessenger;


namespace nexus {

  class BaseGeometry;


  /// User initialization class for detector setup

  class DetectorConstruction: public G4VUserDetectorConstruction
  {
  public:
    /// Constructor
    DetectorConstruction();
    /// Destructor
    ~DetectorConstruction();

    /// Mandatory method invoked by the run manager. 
    /// It returns the physical volume that represents the world.
    virtual G4VPhysicalVolume* Construct();

    /// Set a detector geometry
    void SetGeometry(BaseGeometry*);
    /// Get the detector geometry
    const BaseGeometry* GetGeometry() const;

  private:
    BaseGeometry* _geometry; 
  };


  // INLINE DEFINITIONS /////////////////////////////////////////////
 
  inline void DetectorConstruction::SetGeometry(BaseGeometry* g)
  { _geometry = g; }

  inline const BaseGeometry* DetectorConstruction::GetGeometry() const
  { return _geometry; }

} // end namespace nexus

#endif
