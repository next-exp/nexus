// ----------------------------------------------------------------------------
//  $Id$  Inheriting from previous code in Next1EL.cc
//
//  Author:  <neuslopezmarch@gmail.com>
//
//  Created: 16 Feb 2018
//
//  Copyright (c) 2010, 2018 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com>
//  Date:    June 2019
//  Add "energy_endcap"  &  "tracking_endcap"
// ----------------------------------------------------------------------------


#include "NextDemo.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "DiskSource.h"
#include "Na22Source.h"
#include "NaIScintillator.h"
#include "CylinderPointSampler.h"
#include "MuonsPointSampler.h"
#include "Visibilities.h"

#include <G4Orb.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Region.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>
#include <G4RunManager.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4UnitsTable.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

using namespace nexus;
using namespace CLHEP;

NextDemo::NextDemo():
    BaseGeometry(),
    // LABORATORY //////////////////////////////////
    lab_size_ (2. * m),
    //_rot_angle(pi),
    // VESSEL ////////////////////
    vessel_diam_   (300. * mm),
    vessel_length_ (600. * mm),
    vessel_thickn_ (  3. * mm),
    // ENDCAPS /////////////////////////////////////
    endcap_diam_   (330. * mm),
    endcap_thickn_ ( 30. * mm),
    // SIDE SOURCE-PORT ////////////////////////////
    //  in NextDemoVessel.cc

    /*// AXIAL SOURCE-PORT ///////////////////////////
    axialport_diam_   (16. * mm),
    axialport_length_ (24. * mm),
    axialport_thickn_ ( 2. * mm),
    axialport_flange_diam_   (34. * mm),
    axialport_flange_thickn_ (13. * mm),
    axialport_tube_diam_   ( 5. * mm),
    axialport_tube_length_ (30. * mm),
    axialport_tube_thickn_ ( 1. * mm),
    axialport_tube_window_thickn_ (0.5 * mm),
    */
    ////
    verbosity_(0),
    calib_port_(""),  // ("sideNa")  ("lateral")
    disk_source_(false),  // false, true
    source_mat_(""),    // ("Na")
    ext_scint_(false),    // false, true
    dist_scint_(5. * cm)
{

  //Create geometry-related objects
  vessel_ = new NextDemoVessel(vessel_diam_, vessel_length_, vessel_thickn_);
  inner_elements_ = new NextDemoInnerElements(vessel_length_);

  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
				"Control commands of geometry NextDemo.");

  msg_->DeclareProperty("demo_main_verbosity", verbosity_, "Demo main verbosity");
  // Boolean-type properties (true or false)


  new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

  msg_->DeclareProperty("calib_port", calib_port_, "Where calibration source is placed (lateral/axial/upper)");
  msg_->DeclareProperty("disk_source", disk_source_, "External disk-shape calibration source");
  msg_->DeclareProperty("source_material", source_mat_, "Kind of external disk-shape calibration source");
  msg_->DeclareProperty("ext_scint", ext_scint_, "Placement of external NaI scintillator");

  G4GenericMessenger::Command& scint_dist_cmd =
    msg_->DeclareProperty("scint_distance", dist_scint_,
			  "Distance between the end of the port tube and the NaI scintillator");
    scint_dist_cmd.SetUnitCategory("Length");
    scint_dist_cmd.SetParameterName("scint_distance", false);
    scint_dist_cmd.SetRange("scint_distance>=0.");


  //muons building
  G4GenericMessenger::Command& muonsGenerator_cmd =
    msg_->DeclareProperty("muonsGenerator", muonsGenerator_, "Build or not Muons");
    muonsGenerator_cmd.SetParameterName("muonsGenerator", false);

  naI_ = new NaIScintillator();

  air_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  steel_ = MaterialsList::Steel();

}


NextDemo::~NextDemo()
{
  delete source_gen_side_;
  delete muons_sampling_;
  delete msg_;
  delete vessel_;
  delete inner_elements_;
}


void NextDemo::Construct()
{
  // DefineMaterials();
  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones
  BuildLab();

  if(muonsGenerator_)
    BuildMuons();

  vessel_->Construct();
  G4LogicalVolume* vessel_logic = vessel_->GetLogicalVolume();
  gate_zpos_in_vessel_ = vessel_->GetELzCoord();
  new G4PVPlacement(0, G4ThreeVector(0., 0., -gate_zpos_in_vessel_), vessel_logic,
                    "VESSEL", lab_logic_, false, 0, false);

  G4LogicalVolume*   vessel_internal_logic = vessel_->GetInternalLogicalVolume();
  G4VPhysicalVolume* vessel_internal_phys  = vessel_->GetInternalPhysicalVolume();


  //INNER ELEMENTS
  inner_elements_->SetLogicalVolume(vessel_internal_logic);
  inner_elements_->SetPhysicalVolume(vessel_internal_phys);
  inner_elements_->SetELzCoord(gate_zpos_in_vessel_);
  inner_elements_->Construct();


    //    RUTY     //
    // CATHODE ENDCAP //////////////////////////////////////////////////
    // Flat endcap, CF-300, no ports of interest for the simulation.
/*
        G4Tubs* endcap_cathode_solid =
          new G4Tubs("ENDCAP_CATHODE", 0., endcap_diam_/2.,
                     endcap_thickn_/2., 0., twopi);

        G4LogicalVolume* endcap_cathode_logic =
          new G4LogicalVolume(endcap_cathode_solid, steel_, "ENDCAP_CATHODE");

        G4double posz = (vessel_length_ + endcap_thickn_) / 2.;

        new G4PVPlacement(0, G4ThreeVector(0.,0.,-posz), endcap_cathode_logic,
                          "ENDCAP_CATHODE", lab_logic_, false, 0, true);

        G4VisAttributes * vis1 = new G4VisAttributes;
        vis1->SetColor(0.5, 0.5, .5);
        vis1->SetForceSolid(true);
        endcap_cathode_logic->SetVisAttributes(vis1);


        // ANODE ENDCAP ////////////////////////////////////////////////////
        // Flat endcap, CF-300, CF-16 port for calibration sources
        // Unlike the side-port, union between the endcap and port logical
        // volumes is not required. The endcap will have a central hole
        // where the port will be positioned.

        G4Tubs* endcap_anode_solid =
          // This line in comment If there is no need for the axial port hole.
          //new G4Tubs("ENDCAP_ANODE", (_axialport_diam/2. + _axialport_thickn),
          new G4Tubs("ENDCAP_ANODE", 0.,
                     endcap_diam_/2., endcap_thickn_/2., 0., twopi);

        G4LogicalVolume* endcap_anode_logic =
          new G4LogicalVolume(endcap_anode_solid, steel_, "ENDCAP_ANODE");

        new G4PVPlacement(0, G4ThreeVector(0., 0., posz), endcap_anode_logic,
                          "ENDCAP_ANODE", lab_logic_, false, 0, true);
*/
   // if (verbosity_) {
   //      G4cout << "  ***** ENDCAP_CATHODE *****  " << G4endl;
   //      G4cout << "*** Positions  Z: ***" <<  -posz  << G4endl;
   //      G4cout << "  ***** ENDCAP_ANODE *****  " << G4endl;
   //      G4cout << "*** Positions  Z: ***" <<  posz  << G4endl;
   // }
         /*
        G4VisAttributes * vis2 = new G4VisAttributes;
        vis2->SetColor(0.5, 0.5, .5);
        vis2->SetForceSolid(true);
        endcap_anode_logic->SetVisAttributes(vis2);
         */

  /*// AXIAL SOURCE-PORT ///////////////////////////////////////////////

  // PORT ..................................................

        G4double axialport_total_length = endcap_thickn_ + axialport_length_;

        G4Tubs* axialport_solid =
          new G4Tubs("AXIALPORT", 0., (axialport_diam_/2. + axialport_thickn_),
                     axialport_total_length/2., 0, twopi);

        G4LogicalVolume* axialport_logic =
          new G4LogicalVolume(axialport_solid, steel_, "AXIALPORT");

         posz = (vessel_length_ + axialport_total_length) / 2.;

        axialport_position_.setX(0.);
        axialport_position_.setY(0.);
        axialport_position_.setZ(posz);

        new G4PVPlacement(0, axialport_position_, axialport_logic,
                          "AXIALPORT", lab_logic_, false, 0, true);

      // GAS in Axialport.......................................

        G4Tubs* axialport_gas_solid = new G4Tubs("GAS1", 0., axialport_diam_/2.,
                                                 axialport_total_length/2., 0, twopi);

        G4LogicalVolume* axialport_gas_logic =
          new G4LogicalVolume(axialport_gas_solid, vessel_gas_material, "GAS1");

        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), axialport_gas_logic,
                          "GAS1", axialport_logic, false, 0, true);


        // FLANGE ................................................

        G4Tubs* axialport_flange_solid =
          new G4Tubs("AXIALPORT_FLANGE", axialport_tube_diam_/2.,
                     axialport_flange_diam_/2., axialport_flange_thickn_/2., 0, twopi);

        G4LogicalVolume* axialport_flange_logic =
          new G4LogicalVolume(axialport_flange_solid, steel_, "AXIALPORT_FLANGE");

        // posz = _vessel_length/2. + _endcap_thickn +
        //   _axialport_length + _axialport_flange_thickn/2.;
        //G4double
        posz = vessel_length_/2 + axialport_total_length + axialport_flange_thickn_/2.;

        new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_flange_logic,
                          "AXIALPORT_FLANGE", lab_logic_, false, 0, true);


        // Store the position of the port so that it can be used in vertex generation
        // posz = posz + _axialport_flange_thickn/2.;
        posz = vessel_length_/2. + axialport_total_length + axialport_flange_thickn_;
        axialport_position_.set(0., 0., posz);


        // SOURCE TUBE ...........................................

    G4Tubs* axialport_tube_solid =
      new G4Tubs("AXIALPORT", 0., (axialport_tube_diam_/2.+axialport_tube_thickn_),
                 axialport_tube_length_/2., 0, twopi);

    G4LogicalVolume* axialport_tube_logic =
      new G4LogicalVolume(axialport_tube_solid, steel_, "AXIALPORT");

    //G4double
    posz = (axialport_total_length - axialport_tube_length_) / 2.;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_tube_logic,
                      "AXIALPORT", axialport_gas_logic, false, 0, true);

    G4Tubs* axialport_tube_air_solid =
      new G4Tubs("AXIALPORT_AIR", 0., axialport_tube_diam_/2.,
                 (axialport_tube_length_-axialport_tube_window_thickn_)/2.,
                 0, twopi);

    G4LogicalVolume* axialport_tube_air_logic =
      new G4LogicalVolume(axialport_tubeair__solid, air_, "AXIALPORT_AIR");

    new G4PVPlacement(0, G4ThreeVector(0,0,axialport_tube_window_thickn_/2.),
                      axialport_tube_air_logic, "AXIALPORT_AIR",
                      axialport_tube_logic, false, 0, true);
    */

  //////////////////////////////////////////////////////////////////////////////

  //

  /////////////////////////////////////////////////////////////////////////////////

    G4ThreeVector sideport_pos = vessel_->GetSideSourcePosition(); // this is the position of the end of the port tube
    sideport_ext_position_ = sideport_pos;
    //G4double
    sideport_angle_ =  vessel_->GetSideSourceAngle();
    G4RotationMatrix* sideport_rot = new G4RotationMatrix();
    sideport_rot->rotateY(-pi/2);
    sideport_rot->rotateZ(sideport_angle_);
   if (verbosity_) {
      G4cout << " ********************************* "<< G4endl;
      G4cout << " ********************************* "<< G4endl;
      G4cout << "sideport_pos: " << sideport_pos << G4endl;
      G4cout << "sideport_angle_: " << sideport_angle_ << G4endl;
      G4cout << " ********************************* "<< G4endl;
      G4cout << " ********************************* "<< G4endl;
   }

    if (disk_source_) {
      if (source_mat_ == "Na") {
        source_ = new Na22Source();
      // } else if (_source_mat == "Th") {
      //   _source = new Th228Source();
      } else {
        G4Exception("[NextDemo]", "Construct()", FatalException,
                    "The material of disk source must be Na or Th!");
      }
    // This is the position of the whole source + plastic support.
      if (calib_port_ == "lateral") {
      BuildExtSource(G4ThreeVector(
                        sideport_pos.getX()-(source_->GetSupportThickness()/2.)*cos(sideport_angle_),
                        sideport_pos.getY()-(source_->GetSupportThickness()/2.)*sin(sideport_angle_),
                        sideport_pos.getZ()), *sideport_rot);

      } else {
        G4Exception("[NextDemo]", "Construct()", FatalException,
                    "The placement of disk source must be lateral or upper!");
      }

      //G4VisAttributes light_brown_col = nexus::CopperBrown();
      //source_logic->SetVisAttributes(light_brown_col);
      //source_logic->SetVisAttributes(G4VisAttributes::Invisible);

    }

    // Build NaI external scintillator

    if (ext_scint_) {
      if (calib_port_ == "lateral") {
        // G4RotationMatrix rot;
        // rot.rotateY(-pi/2.);
        BuildExtScintillator(G4ThreeVector(
                       sideport_pos.getX()-(naI_->GetLength()/2.+dist_scint_)*cos(sideport_angle_),
                       sideport_pos.getY()-(naI_->GetLength()/2.+dist_scint_)*sin(sideport_angle_),
                       sideport_pos.getZ()), *sideport_rot);
      /*} else if (calib_port_ == "axial") {
        //    G4RotationMatrix rot_ax;
        BuildExtScintillator(G4ThreeVector(axial_pos.getX(), axial_pos.getY(),
                                           axial_pos.getZ() - dist_scint_ - naI_->GetLength()/2.), *ax_rot); */
      } else {
         G4Exception("[NextDemo]", "Construct()", FatalException,
		  "The placement of external scintillator must be lateral or axial!");
      }
    }


    //// VERTEX GENERATORS   //
    // These are the vertices of the external disk source
    if (disk_source_) {
      //G4double sup_source_diam  = _source->GetSupportDiameter();
      G4double sup_source_thick = source_->GetSupportThickness();
      G4double source_diam = source_->GetSourceDiameter();
      G4double source_thick = source_->GetSourceThickness();
      G4ThreeVector side_pos_gen = G4ThreeVector(
                        //sideport_pos.getX()-(_source->GetSupportThickness())*cos(_sideport_angle),
                        //sideport_pos.getY()-(_source->GetSupportThickness())*sin(_sideport_angle),
                        sideport_pos.getX()-(sup_source_thick-source_thick/2.)*cos(sideport_angle_),
                        sideport_pos.getY()-(sup_source_thick-source_thick/2.)*sin(sideport_angle_),
                        sideport_pos.getZ());

      source_gen_side_ = new CylinderPointSampler(0., source_thick, source_diam/2., 0., side_pos_gen, sideport_rot);

    }

}
/////////////////////////////////////////////////////////////////////////////////

void NextDemo::BuildLab()
{
  // LAB /////////////////////////////////////////////////////////////
  // This is just a volume of air surrounding the detector so that
  // events (from calibration sources or cosmic rays) can be generated
  // on the outside.

  G4Box* lab_solid =
    new G4Box("LAB", lab_size_/2., lab_size_/2., lab_size_/2.);

  lab_logic_ = new G4LogicalVolume(lab_solid, air_, "LAB");
  lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);

  this->SetDrift(true);

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic_);
}


void NextDemo::BuildMuons()
{
  // MUONS /////////////////////////////////////////////////////////////

  G4double xMuons = lab_size_/400;    // origin: lab_size_/3;  /4;
  G4double yMuons = lab_size_/500.;
  G4double zMuons = lab_size_/300;    // origin: lab_size_/2;  /3;

  G4double yMuonsOrigin = 400.;

  //sampling position in a surface above the detector
  muons_sampling_ = new MuonsPointSampler(xMuons, yMuonsOrigin, zMuons);


  // To visualize the muon generation surface

  //visualization sphere
  // G4Orb * muon_solid_ref = new G4Orb ("MUONS_ref",25);
  // G4LogicalVolume*  muon_logic_ref = new G4LogicalVolume(muon_solid_ref, _air, "MUONS_ref");
  // new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic_ref,
  //                             "MUONS_ref", _lab_logic, false, 0, true);

  G4Box* muon_solid =
    new G4Box("MUONS", xMuons, yMuons, zMuons);
  G4LogicalVolume*  muon_logic = new G4LogicalVolume(muon_solid, air_, "MUONS");
  new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic,
		    "MUONS", lab_logic_, false, 0, true);

   // visualization
  G4VisAttributes muon_col = nexus::Red();
  muon_col.SetForceSolid(true);
  muon_logic->SetVisAttributes(muon_col);
}



G4ThreeVector NextDemo::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0., 0., 0.);
  if (region == "MUONS") {
    //generate muons sampling the plane
    vertex = muons_sampling_->GenerateVertex();
  }
    // Extended sources with the shape of a disk outside port
    else if (region == "SOURCE_PORT_LATERAL_DISK") {
    vertex =  source_gen_side_->GenerateVertex("BODY_VOL");
  } else if (region == "SIDEPORT") {
    vertex = sideport_ext_position_;
  //} else if (region == "AXIALPORT") {
  //  vertex = _axialport_position;
  //} else if (region == "Na22LATERAL") {
  //  vertex = _sideNa_pos;
  }

    //INNER ELEMENTS
    else if ( (region == "ACTIVE")  ||
              //(region == "CENTER") ||
	      //(region == "XENON") ||
	      //(region == "DRIFT_TUBE") ||
	      //(region == "BUFFER") ||
              (region == "AD_HOC")  ||
	      (region == "EL_TABLE")
              ){
      vertex = inner_elements_->GenerateVertex(region);
    }
    else if (region == "AD_HOC"){
    //vertex =  _specific_vertex;
    vertex =  vertex;
  }
   else {
      G4Exception("[NextDemo]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

  return vertex;

}


void NextDemo::BuildExtScintillator(G4ThreeVector pos, const G4RotationMatrix& rot)
{
naI_->Construct();
G4LogicalVolume* sc_logic = naI_->GetLogicalVolume();

G4ThreeVector pos_scint =
  G4ThreeVector(pos.getX(), pos.getY(), pos.getZ());
new G4PVPlacement(G4Transform3D(rot, pos_scint), sc_logic, "NaI",
      lab_logic_, false, 0, true);
}



void NextDemo::BuildExtSource(G4ThreeVector pos, const G4RotationMatrix& rot)
{
 source_->Construct();
 G4LogicalVolume* source_logic = source_->GetLogicalVolume();

G4ThreeVector pos_source =
  G4ThreeVector(pos.getX(), pos.getY(), pos.getZ());
new G4PVPlacement(G4Transform3D(rot, pos_source), source_logic, "SOURCE_Na",
      lab_logic_, false, 0, true);
}
