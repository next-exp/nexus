// ----------------------------------------------------------------------------
//  Author: <neuslopezmarch@gmail.com>
//  Created: 16 Feb 2018
//
//  Copyright (c) 2018 NEXT Collaboration
//
//  Updated position and placement, Add choice of Gas
//  Draft by: Ruth Weiss Babai <ruty.wb@gmail.com>
//         Jun 2019 - Apr 2020
//  ************   Need to be chacked   ************
// ----------------------------------------------------------------------------

#include "NextDemoVessel.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "CalibrationSource.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>
#include <G4UnitsTable.hh>
#include <G4Transform3D.hh>
#include <G4SubtractionSolid.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextDemoVessel::NextDemoVessel(const G4double vessel_diam, const G4double vessel_length, const G4double vessel_thickn):

    BaseGeometry(),

    // VESSEL //////////////////////////////////////
    //_vessel_diam   (300. * mm),  // read from NextDemo.cc
    //_vessel_length (600. * mm),  // read from NextDemo.cc
    //_vessel_thickn (  3. * mm),  // read from NextDemo.cc
    // ENDCAPS ///////////////////////////
    //  is constract in NextDemo.cc
    // SIDE SOURCE-PORT ////////////////////////////
    _sideport_diam   (40. * mm),
    _sideport_length (30. * mm), // STEP file: 31.8 
    _sideport_thickn ( 2. * mm),
    _sideport_flange_diam   (71. * mm),
    _sideport_flange_thickn ( 8. * mm),  // STEP file: 43.8 - 31.8 = 12. mm  // prev: 8.
    _sideport_tube_diam   (12.1 * mm),
    _sideport_tube_length (30.0 * mm),
    _sideport_tube_thickn ( 1.0 * mm),
    _sideport_tube_window_thickn (0.5 * mm),
    //_sideport_angle(30. * deg),
    // AXIAL SOURCE-PORT //////////
    //  is constract in NextDemo.cc
    ///////////////////////////////
    // Vessel gas
    _visibility(0),           // 1
    _verbosity(0),
    _pressure(10. * bar),
    _temperature (300 * kelvin),
    _sc_yield(13889/MeV),
    _gas("naturalXe"),
    // _sc_yield(25510. * 1/MeV),
    _Xe_perc(100.)
  {


   /// Needed External variables
   _vessel_diam   = vessel_diam;
   _vessel_length = vessel_length;
   _vessel_thickn = vessel_thickn;

    // Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    _msg->DeclareProperty("vessel_vis", _visibility, "Vessel Visibility");

    _msg->DeclareProperty("vessel_verbosity", _verbosity, "Vessel verbosity");

     G4GenericMessenger::Command& sideport_posz_cmd =
    _msg->DeclareProperty("sideport_posz", _sideport_posz, "Set distance of sideport from z=0.");
     sideport_posz_cmd.SetUnitCategory("Length");

     G4GenericMessenger::Command& sideport_angle_cmd =
    _msg->DeclareProperty("sideport_angle", _sideport_angle, "Set angle of sideport.");
     sideport_angle_cmd.SetUnitCategory("Angle");

    G4GenericMessenger::Command& pressure_cmd =
    _msg->DeclareProperty("pressure", _pressure, "Xenon pressure");
    //"Set pressure for gaseous xenon.");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

    G4GenericMessenger::Command& sc_yield_cmd =
      _msg->DeclareProperty("sc_yield", _sc_yield,
			    "Set scintillation yield for GXe. It is in photons/MeV");
    sc_yield_cmd.SetParameterName("sc_yield", true);
    sc_yield_cmd.SetUnitCategory("1/Energy");

    _msg->DeclareProperty("gas", _gas, "Gas being used");
    _msg->DeclareProperty("XePercentage", _Xe_perc, "Percentage of xenon used in mixtures");
  }



    void NextDemoVessel::Construct()
    {
      //Materials

      G4Material* _air   = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
      G4Material* _steel = MaterialsList::Steel();
      // CAN /////////////////////////////////////////////////////////////
      // Stainless steel (grade 304L) cylinder. CF-300.
      // A CF-60 half-nipple on the side accommodates a window for
      // calibration sources.
  /////////////////////////////////////////////////////////////////////////////////////////////////////
    G4Material* _vessel_gas_mat = nullptr;

    if (_gas == "naturalXe") {
      _vessel_gas_mat = MaterialsList::GXe(_pressure, _temperature);
      _vessel_gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature, _sc_yield));
    } else if (_gas == "enrichedXe") {
      _vessel_gas_mat =  MaterialsList::GXeEnriched(_pressure, _temperature);
      _vessel_gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature, _sc_yield));
    } else if  (_gas == "depletedXe") {
      _vessel_gas_mat =  MaterialsList::GXeDepleted(_pressure, _temperature);
      _vessel_gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature, _sc_yield));
    } else if (_gas == "Ar") {
      _vessel_gas_mat =  MaterialsList::GAr(_pressure, _temperature);
      _vessel_gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GAr(_sc_yield));
    } else if (_gas == "ArXe") {
      _vessel_gas_mat =  MaterialsList::GXeAr(_pressure, _temperature, _Xe_perc);
      _vessel_gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GAr(_sc_yield));
    } else {
      G4Exception("[NextDemoVessel]", "Construct()", FatalException,
		  "Unknown kind of gas, valid options are: naturalXe, enrichedXe, depletedXe, Ar.");
    }

   //////////////////////////////////////////////////////////////////////////

     //   VESSEL      ///////////////////////////////
      // To avoid overlaps between volumes, the can solid volume
      // is the result of the union of the port and the CF-300 cylinder.

     //  VESSEL CAN  ///////////////////////////////////////////////

      G4double vessel_total_diam = _vessel_diam + 2.*_vessel_thickn;

      G4Tubs* can_solid = 
        new G4Tubs("VESSEL", 0., vessel_total_diam/2., _vessel_length/2., 0, twopi);
      // SIDE SOURCE-PORT ////////////////////////////////////////////////

      G4double sideport_total_length = _sideport_length + _vessel_thickn;

      G4Tubs* side_port_solid =
        new G4Tubs("VESSEL_SIDEPORT", 0., (_sideport_diam/2. + _sideport_thickn),
                   sideport_total_length/2., 0, twopi);
     //// FLANGE CF flange closing the port.  ////////     
      G4Tubs* sideport_flange_solid =
        new G4Tubs("SIDEPORT_FLANGE", _sideport_tube_diam/2., _sideport_flange_diam/2.,
                    _sideport_flange_thickn/2., 0, twopi);
      // SIDEPORT_TUBE  ////////
      G4Tubs* sideport_tube_solid =
        new G4Tubs("SIDEPORT_TUBE", 0., (_sideport_tube_diam/2.+_sideport_tube_thickn),
                   _sideport_tube_length/2., 0, twopi);

    ///////  VESSEL GAS  &  SIDE SOURCE-PORT Flange & Tube :: GAS   /////////////
      G4Tubs* cyl_gas =
        new G4Tubs("GAS_VESSEL", 0., _vessel_diam/2., _vessel_length/2., 0, twopi);

      G4Tubs* sideport_gas =
        new G4Tubs("GAS_SIDEPORT", 0., _sideport_diam/2.,
                   sideport_total_length/2., 0, twopi);


    //// UNIONS sideport parts///////

     /*     G4UnionSolid* sideport_solid =
      new G4UnionSolid("VESSEL_SIDEPORT", side_port_solid, sideport_flange_solid, 0,
		       G4ThreeVector(0.,0.,_sideport_total_length/2.+_sideport_flange_thickn/2.));
          sideport_solid =
      new G4UnionSolid("VESSEL_SIDEPORT", sideport_solid, sideport_tube_solid, 0,
		       G4ThreeVector(0.,0.,
                       (_sideport_total_length + _sideport_flange_thickn + _sideport_tube_length)/2.));
       */

            G4UnionSolid* sideport_solid = 
      new G4UnionSolid("VESSEL_SIDEPORT", sideport_flange_solid, sideport_tube_solid, 0,
                       G4ThreeVector(0.,0., _sideport_tube_length/2.));
            sideport_solid =
      new G4UnionSolid("VESSEL_SIDEPORT", sideport_solid, side_port_solid, 0,
                       G4ThreeVector(0.,0., -(sideport_total_length +_sideport_flange_thickn)/2.));



      // rotation matrix for the side source port
      G4RotationMatrix rotport;
      rotport.rotateY(-pi/2.);
      rotport.rotateZ(_sideport_angle);

      // position of the side source-port
      G4double rad_sideport = vessel_total_diam/2. + _sideport_length + _sideport_flange_thickn/2.;
      G4ThreeVector posport(-rad_sideport * cos(_sideport_angle),
                            -rad_sideport * sin(_sideport_angle),
                            _sideport_posz);
      /*G4ThreeVector posport(-(vessel_total_diam+_sideport_length)/2. * cos(_sideport_angle),
                              -(vessel_total_diam+_sideport_length)/2. * sin(_sideport_angle),
                              _sideport_posz); */

      G4UnionSolid* vessel_solid =
        new G4UnionSolid("VESSEL", can_solid, sideport_solid,
                         G4Transform3D(rotport, posport));

      G4LogicalVolume* vessel_logic =
        // new G4LogicalVolume(can_solid, _steel, "VESSEL");
        new G4LogicalVolume(vessel_solid, _steel, "VESSEL");
      this->SetLogicalVolume(vessel_logic);

      ///////////////////////////////////////////////////////////////
      // UNIONS    GAS            /////////////////////////////////
      // Gas filling the can.
      // Obviously, it has the same shape of the can, thus requiring
      // another union between solid volumes.
      ///////////////////////////////////////////////////////////////

      /////////////////////////////   ????   ///////////////////
      posport.setX(-(_vessel_diam + sideport_total_length)/2. * cos(_sideport_angle));
      posport.setY(-(_vessel_diam + sideport_total_length)/2. * sin(_sideport_angle));

      G4UnionSolid* gas_solid = new G4UnionSolid("GAS", cyl_gas, sideport_gas,
                                                 G4Transform3D(rotport, posport));

      G4LogicalVolume* _gas_logic = new G4LogicalVolume(gas_solid, _vessel_gas_mat, "GAS");    // gxe
      _gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

      _internal_logic_vol = _gas_logic;
     
      G4VPhysicalVolume* vessel_gas_phys =
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), _gas_logic,
                          "GAS", vessel_logic, false, 0, true);
      _internal_phys_vol = vessel_gas_phys;


      /*G4VisAttributes * vis = new G4VisAttributes;
      vis->SetColor(0.5, 0.5, .5);
      vis->SetForceSolid(true);
      sideport_flange_logic->SetVisAttributes(vis);
      */

      G4double radial_pos =
      -(vessel_total_diam/2. + _sideport_length + _sideport_flange_thickn/2. + _sideport_tube_length);
      _sideport_ext_position.setX(radial_pos * cos(_sideport_angle));
      _sideport_ext_position.setY(radial_pos * sin(_sideport_angle));
      _sideport_ext_position.setZ(_sideport_posz);


      // TUBE  AIR   ....................................

      radial_pos =
        //-(vessel_total_diam/2. + _sideport_length - _sideport_tube_length/2.);
        -(vessel_total_diam/2. + _sideport_length +_sideport_flange_thickn/2.+ _sideport_tube_length/2.+ _sideport_tube_window_thickn/2.);
      posport.setX(radial_pos * cos(_sideport_angle));
      posport.setY(radial_pos * sin(_sideport_angle));

      _sideport_position = posport;

      G4Tubs* sideport_tube_air_solid =
        new G4Tubs("SIDEPORT_AIR", 0., _sideport_tube_diam/2.,
                   (_sideport_tube_length - _sideport_tube_window_thickn)/2.,
                   0, twopi);

      G4LogicalVolume* sideport_tube_air_logic =
        new G4LogicalVolume(sideport_tube_air_solid, _air, "SIDEPORT_AIR");
    ////////////////////////////////////////////////????????????????????///
      new G4PVPlacement(G4Transform3D(rotport, posport),
                       //(0,G4ThreeVector(0.,0.,_sideport_tube_window_thickn/2.),
                        sideport_tube_air_logic, "SIDEPORT_AIR",
                        vessel_logic, false, 0, true);
                        //sideport_tube_logic, false, 0, true);*/

      /*G4VisAttributes * visvsl = new G4VisAttributes;
      visvsl->SetColor(0.5, 0.5, .5);
      visvsl->SetForceSolid(true);
      vessel_logic->SetVisAttributes(visvsl);

      G4VisAttributes * vis = new G4VisAttributes;
      vis->SetColor(0.9, 0.1, .5);
      vis->SetForceSolid(true);
      sideport_tube_air_logic->SetVisAttributes(vis);
      */
    }

  NextDemoVessel::~NextDemoVessel()
   {}

  G4LogicalVolume* NextDemoVessel::GetInternalLogicalVolume() const
  {
    return _internal_logic_vol;
  }

  G4VPhysicalVolume* NextDemoVessel::GetInternalPhysicalVolume() const
  {
    return _internal_phys_vol;
  }

  G4ThreeVector NextDemoVessel::GetSideSourcePosition() const
  {
    return _sideport_ext_position;
  }

  G4double NextDemoVessel::GetSideSourceAngle() const
  {
    return _sideport_angle;
  }

}//end namespace nexus
