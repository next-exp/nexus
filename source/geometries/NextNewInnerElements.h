// ----------------------------------------------------------------------------
///  \file    NextNewInnerElements 
///  \brief   This class constructs the inner elements of Next-New:
///           Energy plane, field cage and tracking plane.
///  \author   <miquel.nebot@ific.uv.es>
///  \date     18 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_INNER_ELEMENTS__
#define __NEXTNEW_INNER_ELEMENTS__

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;
namespace nexus { class NextNewEnergyPlane; } 
namespace nexus { class NextNewFieldCage; } 
namespace nexus { class NextNewTrackingPlane; }

namespace nexus {

  class NextNewInnerElements: public BaseGeometry
  {

  public:
    // Constructor
    NextNewInnerElements();
    // Destructor
    ~NextNewInnerElements();
    
    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    // It Returns the relative position respect to the rest of NEXTNEW geometry
    //G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


    /// Builder
    void Construct();

  private:
    
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;
    G4double _pressure;
    G4double _temperature;

    // Detector parts
    NextNewEnergyPlane*   _energy_plane;    
    NextNewFieldCage*     _field_cage;
    NextNewTrackingPlane* _tracking_plane;

    // Visibilities
    G4bool _visibility;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 
  
  };

} // end namespace nexus

#endif
    
