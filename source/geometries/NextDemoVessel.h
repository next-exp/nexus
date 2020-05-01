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
    G4LogicalVolume* _internal_logic_vol;
    G4VPhysicalVolume* _internal_phys_vol;

    G4ThreeVector _sideport_position;
    G4ThreeVector _sideport_ext_position;
    // G4ThreeVector _axialport_position;

    G4double _vessel_diam;   ///< Can internal diameter
    G4double _vessel_length; ///< Can internal length <> From NextDemo.cc
    G4double _vessel_thickn; ///< Can thickness
    
    //G4double _endcap_diam;   ///< Vessel endcap diameter
    //G4double _endcap_thickn; ///< Vessel endcap thickness
    
    G4double _sideport_diam;   ///< Side source-port internal diameter
    G4double _sideport_length; ///< Side source-port internal length
    G4double _sideport_thickn; ///< Side source-port thickness
    
    G4double _sideport_flange_diam;   ///< Side-port flange diameter
    G4double _sideport_flange_thickn; ///< Side-port flange thickness
    
    G4double _sideport_tube_diam;   ///< Collimation tube internal diameter
    G4double _sideport_tube_length; ///< Collimation tube internal length
    G4double _sideport_tube_thickn; ///< Collimation tube thickness
    G4double _sideport_tube_window_thickn; ///< Tube window thickness

    G4double _sideport_posz;
    G4double _sideport_angle;
    


    // Visibility
    G4bool _visibility, _verbosity;

    G4double _pressure, _temperature, _sc_yield;
   
     // Gas being used
    G4String _gas;
    G4double _Xe_perc;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

     };

} // end namespace nexus

#endif
 
