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
    _lab_size (2. * m),
    //_rot_angle(pi),
    // VESSEL ////////////////////
    _vessel_diam   (300. * mm),
    _vessel_length (600. * mm),
    _vessel_thickn (  3. * mm),
    // ENDCAPS /////////////////////////////////////
    _endcap_diam   (330. * mm),
    _endcap_thickn ( 30. * mm),
    // SIDE SOURCE-PORT ////////////////////////////
    //  in NextDemoVessel.cc

    /*// AXIAL SOURCE-PORT ///////////////////////////
    _axialport_diam   (16. * mm),
    _axialport_length (24. * mm),
    _axialport_thickn ( 2. * mm),
    _axialport_flange_diam   (34. * mm),
    _axialport_flange_thickn (13. * mm),
    _axialport_tube_diam   ( 5. * mm),
    _axialport_tube_length (30. * mm),
    _axialport_tube_thickn ( 1. * mm),
    _axialport_tube_window_thickn (0.5 * mm),
    */
    ////
    _verbosity(0),
    _calib_port(""),  // ("sideNa")  ("lateral")
    _disk_source(false),  // false, true
    _source_mat(""),    // ("Na")
    _ext_scint(false),    // false, true
    _dist_scint(5. * cm)
{

  //Create geometry-related objects
  _vessel = new NextDemoVessel(_vessel_diam, _vessel_length, _vessel_thickn);
  _inner_elements = new NextDemoInnerElements(_vessel_length);

  _msg = new G4GenericMessenger(this, "/Geometry/NextDemo/",
				"Control commands of geometry NextDemo.");

  _msg->DeclareProperty("demo_main_verbosity", _verbosity, "Demo main verbosity");
  // Boolean-type properties (true or false)


  new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

  _msg->DeclareProperty("calib_port", _calib_port, "Where calibration source is placed (lateral/axial/upper)");
  _msg->DeclareProperty("disk_source", _disk_source, "External disk-shape calibration source");
  _msg->DeclareProperty("source_material", _source_mat, "Kind of external disk-shape calibration source");
  _msg->DeclareProperty("ext_scint", _ext_scint, "Placement of external NaI scintillator");

  G4GenericMessenger::Command& scint_dist_cmd =
    _msg->DeclareProperty("scint_distance", _dist_scint,
			  "Distance between the end of the port tube and the NaI scintillator");
    scint_dist_cmd.SetUnitCategory("Length");
    scint_dist_cmd.SetParameterName("scint_distance", false);
    scint_dist_cmd.SetRange("scint_distance>=0.");


  //muons building
  G4GenericMessenger::Command& muonsGenerator_cmd =
    _msg->DeclareProperty("muonsGenerator", _muonsGenerator, "Build or not Muons");
    muonsGenerator_cmd.SetParameterName("muonsGenerator", false);

  _naI = new NaIScintillator();

  _air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  _steel = MaterialsList::Steel();

}


NextDemo::~NextDemo()
{
  delete _source_gen_side;
  delete _muons_sampling;
  delete _msg;
  delete _vessel;
  delete _inner_elements;
}


void NextDemo::Construct()
{
  // DefineMaterials();
  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones
  BuildLab();

  if(_muonsGenerator)
    BuildMuons();

  //BuildVessel();
  _vessel->Construct();
  G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
  G4ThreeVector position(0.,0.,0.);
  new G4PVPlacement(0, position, vessel_logic,
                    "VESSEL", _lab_logic, false, 0, false);

   G4LogicalVolume* vessel_internal_logic = _vessel->GetInternalLogicalVolume();

   //G4Material* vessel_gas_material = vessel_internal_logic->GetMaterial();


  //INNER ELEMENTS  // Ruty - from New
  _inner_elements->SetLogicalVolume(vessel_internal_logic);
  _inner_elements->Construct();


    //    RUTY     //
    // CATHODE ENDCAP //////////////////////////////////////////////////
    // Flat endcap, CF-300, no ports of interest for the simulation.

        G4Tubs* endcap_cathode_solid =
          new G4Tubs("ENDCAP_CATHODE", 0., _endcap_diam/2.,
                     _endcap_thickn/2., 0., twopi);

        G4LogicalVolume* endcap_cathode_logic =
          new G4LogicalVolume(endcap_cathode_solid, _steel, "ENDCAP_CATHODE");

        G4double posz = (_vessel_length + _endcap_thickn) / 2.;

        new G4PVPlacement(0, G4ThreeVector(0.,0.,-posz), endcap_cathode_logic,
                          "ENDCAP_CATHODE", _lab_logic, false, 0, true);
         /*
        G4VisAttributes * vis1 = new G4VisAttributes;
        vis1->SetColor(0.5, 0.5, .5);
        vis1->SetForceSolid(true);
        endcap_cathode_logic->SetVisAttributes(vis1);
         */

        // ANODE ENDCAP ////////////////////////////////////////////////////
        // Flat endcap, CF-300, CF-16 port for calibration sources
        // Unlike the side-port, union between the endcap and port logical
        // volumes is not required. The endcap will have a central hole
        // where the port will be positioned.

        G4Tubs* endcap_anode_solid =
          // This line in comment If there is no need for the axial port hole.
          //new G4Tubs("ENDCAP_ANODE", (_axialport_diam/2. + _axialport_thickn),
          new G4Tubs("ENDCAP_ANODE", 0.,
                     _endcap_diam/2., _endcap_thickn/2., 0., twopi);

        G4LogicalVolume* endcap_anode_logic =
          new G4LogicalVolume(endcap_anode_solid, _steel, "ENDCAP_ANODE");

        new G4PVPlacement(0, G4ThreeVector(0., 0., posz), endcap_anode_logic,
                          "ENDCAP_ANODE", _lab_logic, false, 0, true);

   if (_verbosity) {
        G4cout << "  ***** ENDCAP_CATHODE *****  " << G4endl;
        G4cout << "*** Positions  Z: ***" <<  -posz  << G4endl;
        G4cout << "  ***** ENDCAP_ANODE *****  " << G4endl;
        G4cout << "*** Positions  Z: ***" <<  posz  << G4endl;
   }
         /*
        G4VisAttributes * vis2 = new G4VisAttributes;
        vis2->SetColor(0.5, 0.5, .5);
        vis2->SetForceSolid(true);
        endcap_anode_logic->SetVisAttributes(vis2);
         */

  /*// AXIAL SOURCE-PORT ///////////////////////////////////////////////

  // PORT ..................................................

        G4double axialport_total_length = _endcap_thickn + _axialport_length;

        G4Tubs* axialport_solid =
          new G4Tubs("AXIALPORT", 0., (_axialport_diam/2. + _axialport_thickn),
                     axialport_total_length/2., 0, twopi);

        G4LogicalVolume* axialport_logic =
          new G4LogicalVolume(axialport_solid, _steel, "AXIALPORT");

         posz = (_vessel_length + axialport_total_length) / 2.;

        _axialport_position.setX(0.);
        _axialport_position.setY(0.);
        _axialport_position.setZ(posz);

        new G4PVPlacement(0, _axialport_position, axialport_logic,
                          "AXIALPORT", _lab_logic, false, 0, true);

      // GAS in Axialport.......................................

        G4Tubs* axialport_gas_solid = new G4Tubs("GAS1", 0., _axialport_diam/2.,
                                                 axialport_total_length/2., 0, twopi);

        G4LogicalVolume* axialport_gas_logic =
          new G4LogicalVolume(axialport_gas_solid, vessel_gas_material, "GAS1");

        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), axialport_gas_logic,
                          "GAS1", axialport_logic, false, 0, true);


        // FLANGE ................................................

        G4Tubs* axialport_flange_solid =
          new G4Tubs("AXIALPORT_FLANGE", _axialport_tube_diam/2.,
                     _axialport_flange_diam/2., _axialport_flange_thickn/2., 0, twopi);

        G4LogicalVolume* axialport_flange_logic =
          new G4LogicalVolume(axialport_flange_solid, _steel, "AXIALPORT_FLANGE");

        // posz = _vessel_length/2. + _endcap_thickn +
        //   _axialport_length + _axialport_flange_thickn/2.;
        //G4double
        posz = _vessel_length/2 + axialport_total_length + _axialport_flange_thickn/2.;

        new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_flange_logic,
                          "AXIALPORT_FLANGE", _lab_logic, false, 0, true);


        // Store the position of the port so that it can be used in vertex generation
        // posz = posz + _axialport_flange_thickn/2.;
        posz = _vessel_length/2. + axialport_total_length + _axialport_flange_thickn;
        _axialport_position.set(0., 0., posz);


        // SOURCE TUBE ...........................................

    G4Tubs* axialport_tube_solid =
      new G4Tubs("AXIALPORT", 0., (_axialport_tube_diam/2.+_axialport_tube_thickn),
                 _axialport_tube_length/2., 0, twopi);

    G4LogicalVolume* axialport_tube_logic =
      new G4LogicalVolume(axialport_tube_solid, _steel, "AXIALPORT");

    //G4double
    posz = (axialport_total_length - _axialport_tube_length) / 2.;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_tube_logic,
                      "AXIALPORT", axialport_gas_logic, false, 0, true);

    G4Tubs* axialport_tube_air_solid =
      new G4Tubs("AXIALPORT_AIR", 0., _axialport_tube_diam/2.,
                 (_axialport_tube_length-_axialport_tube_window_thickn)/2.,
                 0, twopi);

    G4LogicalVolume* axialport_tube_air_logic =
      new G4LogicalVolume(axialport_tube_air_solid, _air, "AXIALPORT_AIR");

    new G4PVPlacement(0, G4ThreeVector(0,0,_axialport_tube_window_thickn/2.),
                      axialport_tube_air_logic, "AXIALPORT_AIR",
                      axialport_tube_logic, false, 0, true);
    */

  //////////////////////////////////////////////////////////////////////////////

  //

  /////////////////////////////////////////////////////////////////////////////////

    G4ThreeVector sideport_pos = _vessel->GetSideSourcePosition(); // this is the position of the end of the port tube
    _sideport_ext_position = sideport_pos;
    //G4double
    _sideport_angle =  _vessel->GetSideSourceAngle();
    G4RotationMatrix* sideport_rot = new G4RotationMatrix();
    sideport_rot->rotateY(-pi/2);
    sideport_rot->rotateZ(_sideport_angle);
   if (_verbosity) {
      G4cout << " ********************************* "<< G4endl;
      G4cout << " ********************************* "<< G4endl;
      G4cout << "sideport_pos: " << sideport_pos << G4endl;
      G4cout << "_sideport_angle: " << _sideport_angle << G4endl;
      G4cout << " ********************************* "<< G4endl;
      G4cout << " ********************************* "<< G4endl;
   }

    if (_disk_source) {
      if (_source_mat == "Na") {
        _source = new Na22Source();
      // } else if (_source_mat == "Th") {
      //   _source = new Th228Source();
      } else {
        G4Exception("[NextDemo]", "Construct()", FatalException,
                    "The material of disk source must be Na or Th!");
      }
    // This is the position of the whole source + plastic support.
      if (_calib_port == "lateral") {
      BuildExtSource(G4ThreeVector(
                        sideport_pos.getX()-(_source->GetSupportThickness()/2.)*cos(_sideport_angle),
                        sideport_pos.getY()-(_source->GetSupportThickness()/2.)*sin(_sideport_angle),
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

    if (_ext_scint) {
      if (_calib_port == "lateral") {
        // G4RotationMatrix rot;
        // rot.rotateY(-pi/2.);
        BuildExtScintillator(G4ThreeVector(
                       sideport_pos.getX()-(_naI->GetLength()/2.+_dist_scint)*cos(_sideport_angle),
                       sideport_pos.getY()-(_naI->GetLength()/2.+_dist_scint)*sin(_sideport_angle),
                       sideport_pos.getZ()), *sideport_rot);
      /*} else if (_calib_port == "axial") {
        //    G4RotationMatrix rot_ax;
        BuildExtScintillator(G4ThreeVector(axial_pos.getX(), axial_pos.getY(),
                                           axial_pos.getZ() - _dist_scint - _naI->GetLength()/2.), *ax_rot); */
      } else {
         G4Exception("[NextDemo]", "Construct()", FatalException,
		  "The placement of external scintillator must be lateral or axial!");
      }
    }


    //// VERTEX GENERATORS   //
    // These are the vertices of the external disk source
    if (_disk_source) {
      //G4double sup_source_diam  = _source->GetSupportDiameter();
      G4double sup_source_thick = _source->GetSupportThickness();
      G4double source_diam = _source->GetSourceDiameter();
      G4double source_thick = _source->GetSourceThickness();
      G4ThreeVector side_pos_gen = G4ThreeVector(
                        //sideport_pos.getX()-(_source->GetSupportThickness())*cos(_sideport_angle),
                        //sideport_pos.getY()-(_source->GetSupportThickness())*sin(_sideport_angle),
                        sideport_pos.getX()-(sup_source_thick-source_thick/2.)*cos(_sideport_angle),
                        sideport_pos.getY()-(sup_source_thick-source_thick/2.)*sin(_sideport_angle),
                        sideport_pos.getZ());

      _source_gen_side = new CylinderPointSampler(0., source_thick, source_diam/2., 0., side_pos_gen, sideport_rot);

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
    new G4Box("LAB", _lab_size/2., _lab_size/2., _lab_size/2.);

  _lab_logic = new G4LogicalVolume(lab_solid, _air, "LAB");
  _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

  this->SetDrift(true);

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(_lab_logic);
}


void NextDemo::BuildMuons()
{
  // MUONS /////////////////////////////////////////////////////////////

  G4double xMuons = _lab_size/400;    // origin: _lab_size/3;  /4;
  G4double yMuons = _lab_size/500.;
  G4double zMuons = _lab_size/300;    // origin: _lab_size/2;  /3;

  G4double yMuonsOrigin = 400.;

  //sampling position in a surface above the detector
  _muons_sampling = new MuonsPointSampler(xMuons, yMuonsOrigin, zMuons);


  // To visualize the muon generation surface

  //visualization sphere
  // G4Orb * muon_solid_ref = new G4Orb ("MUONS_ref",25);
  // G4LogicalVolume*  muon_logic_ref = new G4LogicalVolume(muon_solid_ref, _air, "MUONS_ref");
  // new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic_ref,
  //                             "MUONS_ref", _lab_logic, false, 0, true);

  G4Box* muon_solid =
    new G4Box("MUONS", xMuons, yMuons, zMuons);
  G4LogicalVolume*  muon_logic = new G4LogicalVolume(muon_solid, _air, "MUONS");
  new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic,
		    "MUONS", _lab_logic, false, 0, true);

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
    vertex = _muons_sampling->GenerateVertex();
  }
    // Extended sources with the shape of a disk outside port
    else if (region == "SOURCE_PORT_LATERAL_DISK") {
    vertex =  _source_gen_side->GenerateVertex("BODY_VOL");
  } else if (region == "SIDEPORT") {
    vertex = _sideport_ext_position;
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
      vertex = _inner_elements->GenerateVertex(region);
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
_naI->Construct();
G4LogicalVolume* sc_logic = _naI->GetLogicalVolume();

G4ThreeVector pos_scint =
  G4ThreeVector(pos.getX(), pos.getY(), pos.getZ());
new G4PVPlacement(G4Transform3D(rot, pos_scint), sc_logic, "NaI",
      _lab_logic, false, 0, true);
}



void NextDemo::BuildExtSource(G4ThreeVector pos, const G4RotationMatrix& rot)
{
 _source->Construct();
 G4LogicalVolume* source_logic = _source->GetLogicalVolume();

G4ThreeVector pos_source =
  G4ThreeVector(pos.getX(), pos.getY(), pos.getZ());
new G4PVPlacement(G4Transform3D(rot, pos_source), source_logic, "SOURCE_Na",
      _lab_logic, false, 0, true);
}


