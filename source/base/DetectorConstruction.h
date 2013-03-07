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

#ifndef __DETECTOR_CONSTRUCTION__
#define __DETECTOR_CONSTRUCTION__

#include <G4VUserDetectorConstruction.hh>
#include "BaseGeometry.h"

class G4GenericMessenger;


namespace nexus {

  
  class DetectorConstruction: public G4VUserDetectorConstruction
  {
  public:
    /// Constructor
    DetectorConstruction();
    /// Destructor
    ~DetectorConstruction();

    /// Mandatory method invoked by the G4RunManager. It returns 
    /// the physical volume that represents the world.
    G4VPhysicalVolume* Construct();

    /// Set a detector geometry
    void SetGeometry(BaseGeometry*);
    /// Get the detector geometry
    const BaseGeometry* GetGeometry() const;

  private:
    //G4GenericMessenger* _msg;
    BaseGeometry* _geometry; ///< pointer to the detector geometry
  };

  // inline methods ..................................................
  
  inline void DetectorConstruction::SetGeometry(BaseGeometry* g)
  { _geometry = g; }

  inline const BaseGeometry* DetectorConstruction::GetGeometry() const
  { return _geometry; }


} // end namespace nexus

#endif
