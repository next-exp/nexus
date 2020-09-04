// ----------------------------------------------------------------------------
//  Next Demo++ Vessel
//
//  Author:   <neuslopezmarch@gmail.com>
//  Crated: 16 Feb 2018
//
//  Copyright (c) 2018 NEXT Collaboration
//
//  Updated position and placement, Add choice of Gas
//  Draft by: Ruth Weiss Babai <ruty.wb@gmail.com>
//         Jun 2019 - Apr 2020
//  ************   Need to be chacked   ************
// ----------------------------------------------------------------------------

#ifndef __NEXT_DEMO_VESSEL__
#define __NEXT_DEMO_VESSEL__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
//#include "SpherePointSampler.h"


#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class CalibrationSource;

  class NextDemoVessel: public BaseGeometry
  {
  public:
    /// Constructor
    NextDemoVessel(const G4double vessel_diam, const G4double vessel_length, const G4double vessel_thickn);
    ///Destructor
    ~NextDemoVessel();
    
    /// Returns the logical volume of the inner object

    G4LogicalVolume* GetInternalLogicalVolume() const;

    G4VPhysicalVolume* GetInternalPhysicalVolume() const;

    G4ThreeVector GetSideSourcePosition() const;

    G4double GetSideSourceAngle() const;

    /// Builder
    void Construct();

  private:
    // Internal Logical Volume
    G4LogicalVolume* internal_logic_vol_;
    G4VPhysicalVolume* internal_phys_vol_;

    G4ThreeVector sideport_position_;
    G4ThreeVector sideport_ext_position_;
    // G4ThreeVector _axialport_position;

    G4double vessel_diam_;   ///< Can internal diameter
    G4double vessel_length_; ///< Can internal length <> From NextDemo.cc
    G4double vessel_thickn_; ///< Can thickness
    
    //G4double _endcap_diam;   ///< Vessel endcap diameter
    //G4double _endcap_thickn; ///< Vessel endcap thickness
    
    G4double sideport_diam_;   ///< Side source-port internal diameter
    G4double sideport_length_; ///< Side source-port internal length
    G4double sideport_thickn_; ///< Side source-port thickness
    
    G4double sideport_flange_diam_;   ///< Side-port flange diameter
    G4double sideport_flange_thickn_; ///< Side-port flange thickness
    
    G4double sideport_tube_diam_;   ///< Collimation tube internal diameter
    G4double sideport_tube_length_; ///< Collimation tube internal length
    G4double sideport_tube_thickn_; ///< Collimation tube thickness
    G4double sideport_tube_window_thickn_; ///< Tube window thickness

    G4double sideport_posz_;
    G4double sideport_angle_;
    


    // Visibility
    G4bool visibility_, verbosity_;

    G4double pressure_, temperature_, sc_yield_;
   
     // Gas being used
    G4String gas_;
    G4double Xe_perc_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

     };

} // end namespace nexus

#endif
 
