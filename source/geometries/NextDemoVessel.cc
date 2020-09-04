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
    sideport_diam_   (40. * mm),
    sideport_length_ (30. * mm), // STEP file: 31.8 
    sideport_thickn_ ( 2. * mm),
    sideport_flange_diam_   (71. * mm),
    sideport_flange_thickn_ ( 8. * mm),  // STEP file: 43.8 - 31.8 = 12. mm  // prev: 8.
    sideport_tube_diam_   (12.1 * mm),
    sideport_tube_length_ (30.0 * mm),
    sideport_tube_thickn_ ( 1.0 * mm),
    sideport_tube_window_thickn_ (0.5 * mm),
    //_sideport_angle(30. * deg),
    // AXIAL SOURCE-PORT //////////
    //  is constract in NextDemo.cc
    ///////////////////////////////
    // Vessel gas
    visibility_(0),           // 1
    verbosity_(0),
    pressure_(10. * bar),
    temperature_ (300 * kelvin),
    sc_yield_(13889/MeV),
    gas_("naturalXe"),
    // _sc_yield(25510. * 1/MeV),
    Xe_perc_(100.)
  {


   /// Needed External variables
   vessel_diam_   = vessel_diam;
   vessel_length_ = vessel_length;
   vessel_thickn_ = vessel_thickn;

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    msg_->DeclareProperty("vessel_vis", visibility_, "Vessel Visibility");

    msg_->DeclareProperty("vessel_verbosity", verbosity_, "Vessel verbosity");

     G4GenericMessenger::Command& sideport_posz_cmd =
    msg_->DeclareProperty("sideport_posz", sideport_posz_, "Set distance of sideport from z=0.");
     sideport_posz_cmd.SetUnitCategory("Length");

     G4GenericMessenger::Command& sideport_angle_cmd =
    msg_->DeclareProperty("sideport_angle", sideport_angle_, "Set angle of sideport.");
     sideport_angle_cmd.SetUnitCategory("Angle");

    G4GenericMessenger::Command& pressure_cmd =
    msg_->DeclareProperty("pressure", pressure_, "Xenon pressure");
    //"Set pressure for gaseous xenon.");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

    G4GenericMessenger::Command& sc_yield_cmd =
      msg_->DeclareProperty("sc_yield", sc_yield_,
			    "Set scintillation yield for GXe. It is in photons/MeV");
    sc_yield_cmd.SetParameterName("sc_yield", true);
    sc_yield_cmd.SetUnitCategory("1/Energy");

    msg_->DeclareProperty("gas", gas_, "Gas being used");
    msg_->DeclareProperty("XePercentage", Xe_perc_, "Percentage of xenon used in mixtures");
  }



    void NextDemoVessel::Construct()
    {
      //Materials

      G4Material* air_   = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
      G4Material* steel_ = MaterialsList::Steel();
      // CAN /////////////////////////////////////////////////////////////
      // Stainless steel (grade 304L) cylinder. CF-300.
      // A CF-60 half-nipple on the side accommodates a window for
      // calibration sources.
  /////////////////////////////////////////////////////////////////////////////////////////////////////
    G4Material* vessel_gas_mat_ = nullptr;

    if (gas_ == "naturalXe") {
      vessel_gas_mat_ = MaterialsList::GXe(pressure_, temperature_);
      vessel_gas_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(pressure_, temperature_, sc_yield_));
    } else if (gas_ == "enrichedXe") {
      vessel_gas_mat_ =  MaterialsList::GXeEnriched(pressure_, temperature_);
      vessel_gas_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(pressure_, temperature_, sc_yield_));
    } else if  (gas_ == "depletedXe") {
      vessel_gas_mat_ =  MaterialsList::GXeDepleted(pressure_, temperature_);
      vessel_gas_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(pressure_, temperature_, sc_yield_));
    } else if (gas_ == "Ar") {
      vessel_gas_mat_ =  MaterialsList::GAr(pressure_, temperature_);
      vessel_gas_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::GAr(sc_yield_));
    } else if (gas_ == "ArXe") {
      vessel_gas_mat_ =  MaterialsList::GXeAr(pressure_, temperature_, Xe_perc_);
      vessel_gas_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::GAr(sc_yield_));
    } else {
      G4Exception("[NextDemoVessel]", "Construct()", FatalException,
		  "Unknown kind of gas, valid options are: naturalXe, enrichedXe, depletedXe, Ar.");
    }

   //////////////////////////////////////////////////////////////////////////

     //   VESSEL      ///////////////////////////////
      // To avoid overlaps between volumes, the can solid volume
      // is the result of the union of the port and the CF-300 cylinder.

     //  VESSEL CAN  ///////////////////////////////////////////////

      G4double vessel_total_diam = vessel_diam_ + 2.*vessel_thickn_;

      G4Tubs* can_solid = 
        new G4Tubs("VESSEL", 0., vessel_total_diam/2., vessel_length_/2., 0, twopi);
      // SIDE SOURCE-PORT ////////////////////////////////////////////////

      G4double sideport_total_length = sideport_length_ + vessel_thickn_;

      G4Tubs* side_port_solid =
        new G4Tubs("VESSEL_SIDEPORT", 0., (sideport_diam_/2. + sideport_thickn_),
                   sideport_total_length/2., 0, twopi);
     //// FLANGE CF flange closing the port.  ////////     
      G4Tubs* sideport_flange_solid =
        new G4Tubs("SIDEPORT_FLANGE", sideport_tube_diam_/2., sideport_flange_diam_/2.,
                    sideport_flange_thickn_/2., 0, twopi);
      // SIDEPORT_TUBE  ////////
      G4Tubs* sideport_tube_solid =
        new G4Tubs("SIDEPORT_TUBE", 0., (sideport_tube_diam_/2.+sideport_tube_thickn_),
                   sideport_tube_length_/2., 0, twopi);

    ///////  VESSEL GAS  &  SIDE SOURCE-PORT Flange & Tube :: GAS   /////////////
      G4Tubs* cyl_gas =
        new G4Tubs("GAS_VESSEL", 0., vessel_diam_/2., vessel_length_/2., 0, twopi);

      G4Tubs* sideport_gas =
        new G4Tubs("GAS_SIDEPORT", 0., sideport_diam_/2.,
                   sideport_total_length/2., 0, twopi);


    //// UNIONS sideport parts///////

     /*     G4UnionSolid* sideport_solid =
      new G4UnionSolid("VESSEL_SIDEPORT", side_port_solid, sideport_flange_solid, 0,
		       G4ThreeVector(0.,0.,sideport_total_length_/2.+sideport_flange_thickn_/2.));
          sideport_solid =
      new G4UnionSolid("VESSEL_SIDEPORT", sideport_solid, sideport_tube_solid, 0,
		       G4ThreeVector(0.,0.,
                       (sideport_total_length_ + sideport_flange_thickn_ + sideport_tube_length_)/2.));
       */

            G4UnionSolid* sideport_solid = 
      new G4UnionSolid("VESSEL_SIDEPORT", sideport_flange_solid, sideport_tube_solid, 0,
                       G4ThreeVector(0.,0., sideport_tube_length_/2.));
            sideport_solid =
      new G4UnionSolid("VESSEL_SIDEPORT", sideport_solid, side_port_solid, 0,
                       G4ThreeVector(0.,0., -(sideport_total_length +sideport_flange_thickn_)/2.));



      // rotation matrix for the side source port
      G4RotationMatrix rotport;
      rotport.rotateY(-pi/2.);
      rotport.rotateZ(sideport_angle_);

      // position of the side source-port
      G4double rad_sideport = vessel_total_diam/2. + sideport_length_ + sideport_flange_thickn_/2.;
      G4ThreeVector posport(-rad_sideport * cos(sideport_angle_),
                            -rad_sideport * sin(sideport_angle_),
                            sideport_posz_);
      /*G4ThreeVector posport(-(vessel_total_diam+sideport_length_)/2. * cos(sideport_angle_),
                              -(vessel_total_diam+sideport_length_)/2. * sin(sideport_angle_),
                              sideport_posz_); */

      G4UnionSolid* vessel_solid =
        new G4UnionSolid("VESSEL", can_solid, sideport_solid,
                         G4Transform3D(rotport, posport));

      G4LogicalVolume* vessel_logic =
        // new G4LogicalVolume(can_solid, _steel, "VESSEL");
        new G4LogicalVolume(vessel_solid, steel_, "VESSEL");
      this->SetLogicalVolume(vessel_logic);

      ///////////////////////////////////////////////////////////////
      // UNIONS    GAS            /////////////////////////////////
      // Gas filling the can.
      // Obviously, it has the same shape of the can, thus requiring
      // another union between solid volumes.
      ///////////////////////////////////////////////////////////////

      /////////////////////////////   ????   ///////////////////
      posport.setX(-(vessel_diam_ + sideport_total_length)/2. * cos(sideport_angle_));
      posport.setY(-(vessel_diam_ + sideport_total_length)/2. * sin(sideport_angle_));

      G4UnionSolid* gas_solid = new G4UnionSolid("GAS", cyl_gas, sideport_gas,
                                                 G4Transform3D(rotport, posport));

      G4LogicalVolume* gas_logic_ = new G4LogicalVolume(gas_solid, vessel_gas_mat_, "GAS");    // gxe
      gas_logic_->SetVisAttributes(G4VisAttributes::Invisible);

      internal_logic_vol_ = gas_logic_;
     
      G4VPhysicalVolume* vessel_gas_phys =
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic_,
                          "GAS", vessel_logic, false, 0, true);
      internal_phys_vol_ = vessel_gas_phys;


      /*G4VisAttributes * vis = new G4VisAttributes;
      vis->SetColor(0.5, 0.5, .5);
      vis->SetForceSolid(true);
      sideport_flange_logic->SetVisAttributes(vis);
      */

      G4double radial_pos =
      -(vessel_total_diam/2. + sideport_length_ + sideport_flange_thickn_/2. + sideport_tube_length_);
      sideport_ext_position_.setX(radial_pos * cos(sideport_angle_));
      sideport_ext_position_.setY(radial_pos * sin(sideport_angle_));
      sideport_ext_position_.setZ(sideport_posz_);


      // TUBE  AIR   ....................................

      radial_pos =
        //-(vessel_total_diam/2. + _sideport_length - _sideport_tube_length/2.);
        -(vessel_total_diam/2. + sideport_length_ +sideport_flange_thickn_/2.+ sideport_tube_length_/2.+ sideport_tube_window_thickn_/2.);
      posport.setX(radial_pos * cos(sideport_angle_));
      posport.setY(radial_pos * sin(sideport_angle_));

      sideport_position_ = posport;

      G4Tubs* sideport_tube_air_solid =
        new G4Tubs("SIDEPORT_AIR", 0., sideport_tube_diam_/2.,
                   (sideport_tube_length_ - sideport_tube_window_thickn_)/2.,
                   0, twopi);

      G4LogicalVolume* sideport_tube_air_logic =
        new G4LogicalVolume(sideport_tube_air_solid, air_, "SIDEPORT_AIR");
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
    return internal_logic_vol_;
  }

  G4VPhysicalVolume* NextDemoVessel::GetInternalPhysicalVolume() const
  {
    return internal_phys_vol_;
  }

  G4ThreeVector NextDemoVessel::GetSideSourcePosition() const
  {
    return sideport_ext_position_;
  }

  G4double NextDemoVessel::GetSideSourceAngle() const
  {
    return sideport_angle_;
  }

}//end namespace nexus
