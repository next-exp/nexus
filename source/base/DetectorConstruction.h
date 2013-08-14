// ----------------------------------------------------------------------------
///  \file   DetectorConstruction.h
///  \brief  Initialization class for detector setup.
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     9 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __DETECTOR_CONSTRUCTION__
#define __DETECTOR_CONSTRUCTION__

#include <G4VUserDetectorConstruction.hh>
#include "BaseGeometry.h"


namespace nexus {

  /// Geant4 user initialization class for detector setup.
  /// A detector geometry selected by the user and created (allocated)
  /// by the NexusFactory is set in the world. 
  /// This class owns the geometry instance and takes care of deleting 
  /// it at the end of the run.
  
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
    BaseGeometry* _geometry; ///< pointer to the detector geometry
  };

  // inline methods ..................................................
  
  inline void DetectorConstruction::SetGeometry(BaseGeometry* g)
  { _geometry = g; }

  inline const BaseGeometry* DetectorConstruction::GetGeometry() const
  { return _geometry; }


} // end namespace nexus

#endif
