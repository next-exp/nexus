// ------------------------------------------------------------------------
///  \file    NextNewInnerElements
///  \brief   This class constructs the inner elements of Next-New:
///           Energy plane, field cage and tracking plane.
///  \author   <miquel.nebot@ific.uv.es>
///  \date     18 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
//
//
//  Updated to NextDemo++  by  Ruth Weiss Babai  <ruty.wb@gmail.com>
//  From: NextNewInnerElements.h
//  Date:      June-Aug 2019
// ------------------------------------------------------------------------

#ifndef __NEXT_DEMO_INNER_ELEMENTS__
#define __NEXT_DEMO_INNER_ELEMENTS__

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;

namespace nexus { class NextDemoFieldCage; }
namespace nexus { class NextDemoTrackingPlane; }
namespace nexus { class NextDemoEnergyPlane; }

namespace nexus {

  class NextDemoInnerElements: public BaseGeometry
  {

  public:
    // Constructor
    NextDemoInnerElements(const G4double vessel_length);
    // Destructor
    ~NextDemoInnerElements();

    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets the physical volume, needed for the definition
    /// of border optical surfaces
    void SetPhysicalVolume(G4VPhysicalVolume* mother_phys);


    // It Returns the relative position respect to the rest of NEXTNEW geometry
    //G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


    /// Builder
    void Construct();

  private:

    G4double gate_sapphire_wdw_distance_;

    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;

    G4Material* gas_;
    G4double pressure_;
    G4double temperature_;

    // Can internal length <> From NextDemo.cc -> transfer to NextDemoFieldCage.cc
    G4double vessel_length_;

    // Detector parts
    NextDemoFieldCage*     field_cage_;
    NextDemoTrackingPlane* tracking_plane_;
    NextDemoEnergyPlane*   energy_plane_;

    // Visibilities or Verbosity
    // G4bool _visibility;
    G4bool verbosity_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
