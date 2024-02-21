// ----------------------------------------------------------------------------
// nexus | Next1EL.cc
//
// Geometry of the NEXT-DEMO detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next1EL.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtR7378A.h"
#include "IonizationSD.h"
#include "HexagonPointSampler.h"
#include "UniformElectricDriftField.h"
#include "XenonProperties.h"
#include "NextElDB.h"
#include "CylinderPointSamplerLegacy.h"
#include "BoxPointSamplerLegacy.h"
#include "Visibilities.h"
#include "FactoryBase.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4Polyhedra.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>
#include <G4RunManager.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(Next1EL, GeometryBase)

Next1EL::Next1EL():
  GeometryBase(),
  // LABORATORY //////////////////////////////////
  lab_size_ (2. * m),
  // VESSEL //////////////////////////////////////
  vessel_diam_   (300. * mm),
  vessel_length_ (600. * mm),
  vessel_thickn_ (  3. * mm),
  // ENDCAPS /////////////////////////////////////
  endcap_diam_   (330. * mm),
  endcap_thickn_ ( 30. * mm),
  // SIDE SOURCE-PORT ////////////////////////////
  sideport_diam_   (40. * mm),
  sideport_length_ (30. * mm),
  sideport_thickn_ ( 2. * mm),
  sideport_flange_diam_   (71. * mm),
  sideport_flange_thickn_ ( 8. * mm),
  sideport_tube_diam_   (12.1 * mm),
  sideport_tube_length_ (30.0 * mm),
  sideport_tube_thickn_ ( 1.0 * mm),
  sideport_tube_window_thickn_ (0.5 * mm),
  // AXIAL SOURCE-PORT ///////////////////////////
  axialport_diam_   (16. * mm),
  axialport_length_ (24. * mm),
  axialport_thickn_ ( 2. * mm),
  axialport_flange_diam_   (34. * mm),
  axialport_flange_thickn_ (13. * mm),
  axialport_tube_diam_   ( 5. * mm),
  axialport_tube_length_ (30. * mm),
  axialport_tube_thickn_ ( 1. * mm),
  axialport_tube_window_thickn_ (0.5 * mm),
  // ELECTROLUMINESCENCE GAP /////////////////////
  elgap_length_ (5. * mm),
  elgap_ring_diam_   (229. * mm),
  elgap_ring_thickn_ ( 12. * mm),
  elgap_ring_height_ (  5. * mm),
  // wire_diam_(.003048 * cm),
  // TPB
  tpb_thickn_ (.001 * mm),
  // LIGHT TUBE //////////////////////////////////
  ltube_diam_      (160. * mm),
  ltube_thickn_    (  5. * mm + tpb_thickn_),
  ltube_up_length_ (295. * mm),
  ltube_bt_length_ (100. * mm),
  ltube_gap_       (  3. * mm),
  // ACTIVE VOLUME ///////////////////////////////
  active_diam_   (ltube_diam_),
  active_length_ (300. * mm),
  // FIELD CAGE //////////////////////////////////
  fieldcage_length_ (elgap_ring_height_ + elgap_length_ +
		     active_length_ + ltube_gap_ +
		     ltube_bt_length_),
  fieldcage_displ_  (84.*mm),
  // FIELD SHAPING RINGS /////////////////////////
  ring_diam_   (229. * mm),
  // ring_height_ ( 10. * mm),
  ring_thickn_ (  6. * mm),
  // SUPPORT BARS ////////////////////////////////
  bar_width_  ( 40. * mm),
  bar_thickn_ ( 10. * mm),
  bar_addon_length_ (50. * mm),
  // PMT ENERGY PLANE ////////////////////////////
  pmtholder_cath_diam_         (229. * mm),
  pmtholder_cath_height_       ( 40. * mm),
  pmtholder_cath_cutout_depth_ ( 10. * mm),
  pmtholder_cath_displ_ (1. * cm),
  pmt_pitch_ (34.59 * mm),
  // PMT TRACKING PLANE //////////////////////////
  pmtholder_anode_diam_   (229. * mm),
  pmtholder_anode_thickn_ ( 13. * mm),
  // DEFAULT VALUES FOR SOME PARAMETERS///////////
  max_step_size_(1.*mm),
  sc_yield_(13889/MeV),
  e_lifetime_(1000.*ms),
  specific_vertex_{}
{

  msg_ = new G4GenericMessenger(this, "/Geometry/Next1EL/",
				"Control commands of geometry Next1EL.");

  // Boolean-type properties (true or false)
  msg_->DeclareProperty("elfield", elfield_,
			"True if the EL field is on (full simulation), false if it's not (parametrized simulation.");
  msg_->DeclareProperty("tpb_coating", tpb_coating_,
			"True if the upper light tube is coated.");
  msg_->DeclareProperty("external_scintillator", external_scintillator_,
			"True if the an external NaI scintillator is used.");


  // String-like properties
  msg_->DeclareProperty("tracking_plane", tracking_plane_,
			"PMT or SIPM, according to what tracking plane it's being used.");

  new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

  G4GenericMessenger::Command& sc_yield_cmd =
    msg_->DeclareProperty("sc_yield", sc_yield_,
			  "Set scintillation yield for GXe. It is in photons/MeV");
  sc_yield_cmd.SetParameterName("sc_yield", true);
  sc_yield_cmd.SetUnitCategory("1/Energy");

  G4GenericMessenger::Command& e_lifetime_cmd =
    msg_->DeclareProperty("e_lifetime", e_lifetime_,
        "Electron lifetime in gas.");
  e_lifetime_cmd.SetParameterName("e_lifetime", false);
  e_lifetime_cmd.SetUnitCategory("Time");
  e_lifetime_cmd.SetRange("e_lifetime>0.");

  msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
      "Set generation vertex.");

   // Other properties (dimension and dimensionless)
  G4GenericMessenger::Command& pressure_cmd =
    msg_->DeclareProperty("pressure", pressure_,
			  "Set pressure for gaseous xenon.");
  pressure_cmd.SetUnitCategory("Pressure");
  pressure_cmd.SetParameterName("pressure", false);
  pressure_cmd.SetRange("pressure>0.");

  G4GenericMessenger::Command& sideport_angle_cmd =
    msg_->DeclareProperty("sideport_angle", sideport_angle_,
  			  "Set angle of sideport.");
  sideport_angle_cmd.SetUnitCategory("Angle");

  G4GenericMessenger::Command& sideport_posz_cmd =
    msg_->DeclareProperty("sideport_posz", sideport_posz_,
			  "Set distance of sideport from z=0.");
  sideport_posz_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& max_step_size_cmd =
    msg_->DeclareProperty("max_step_size", max_step_size_,
			  "Set maximum step size.");
  max_step_size_cmd.SetUnitCategory("Length");
  max_step_size_cmd.SetParameterName("max_step_size", true);
  max_step_size_cmd.SetRange("max_step_size>0");

  G4GenericMessenger::Command& elgrid_transparency_cmd =
    msg_->DeclareProperty("elgrid_transparency", elgrid_transparency_,
			  "Set the transparency of the anode EL mesh.");
  elgrid_transparency_cmd.SetParameterName("elgrid_transparency", false);
  elgrid_transparency_cmd.SetRange("elgrid_transparency>0 && elgrid_transparency<1");

  G4GenericMessenger::Command& gate_transparency_cmd =
    msg_->DeclareProperty("gate_transparency", gate_transparency_,
			  "Set the transparency of the gate EL mesh.");
  gate_transparency_cmd.SetParameterName("gate_transparency", false);
  gate_transparency_cmd.SetRange("gate_transparency>0 && gate_transparency<1");


  //muons building
  G4GenericMessenger::Command& muonsGenerator_cmd =msg_->DeclareProperty("muonsGenerator", muonsGenerator_,
			"Build or not Muons");
  muonsGenerator_cmd.SetParameterName("muonsGenerator", false);

}



Next1EL::~Next1EL()
{
  delete hexrnd_;
  delete muons_sampling_;
  delete msg_;
  delete cps_;
}


void Next1EL::Construct()
{
  DefineMaterials();
  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones
  BuildLab();
  if(muonsGenerator_)
    BuildMuons();
  if (external_scintillator_)
    BuildExtScintillator();
  BuildVessel();
  BuildFieldCage();
  BuildEnergyPlane();

  if (tracking_plane_ == "SIPM")
    BuildSiPMTrackingPlane();
  else
    BuildPMTTrackingPlane();

   // For EL Table generation
  idx_table_ = 0;
  table_vertices_.clear();
  /// Ionielectrons are generated at a z = .5 mm inside the EL gap
  G4double z = vessel_length_/2. - fieldcage_displ_ - elgap_ring_height_ - elgap_length_;
  SetELzCoord(z);
  z = z +  .5*mm;
  CalculateELTableVertices(92.5*mm, 5.*mm, z);

}




void Next1EL::DefineMaterials()
{
  // AIR
  air_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  air_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  // GASEOUS XENON
  gxe_ = materials::GXe(pressure_, 303);
  gxe_->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, 303, sc_yield_, e_lifetime_));
  // PTFE (TEFLON)
  teflon_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  teflon_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  // STAINLESS STEEL
  steel_ = materials::Steel();
  steel_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  // ALUMINUM
  aluminum_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  aluminum_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  //LEAD
  lead_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
  lead_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  //PLASTIC
  plastic_ = materials::PS();
  plastic_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  //TPB
  tpb_ = materials::TPB();
  tpb_->SetMaterialPropertiesTable(opticalprops::TPB());
}



void Next1EL::BuildLab()
{
  // LAB /////////////////////////////////////////////////////////////
  // This is just a volume of air surrounding the detector so that
  // events (from calibration sources or cosmic rays) can be generated
  // on the outside.

  G4Box* lab_solid =
    new G4Box("LAB", lab_size_/2., lab_size_/2., lab_size_/2.);

  lab_logic_ = new G4LogicalVolume(lab_solid, air_, "LAB");
  lab_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic_);
}

void Next1EL::BuildMuons()
{
  // MUONS /////////////////////////////////////////////////////////////

  G4double xMuons =  lab_size_/3;
  G4double yMuons = lab_size_/500.;
  G4double zMuons = lab_size_/2;

  G4double yMuonsOrigin = 400.;

  //sampling position in a surface above the detector
  muons_sampling_ =
    new BoxPointSamplerLegacy(xMuons*2., 0, zMuons*2., 0,
                              G4ThreeVector(0, yMuonsOrigin, 0));


  // To visualize the muon generation surface

  //visualization sphere
  // G4Orb * muon_solid_ref = new G4Orb ("MUONS_ref",25);
  // G4LogicalVolume*  muon_logic_ref = new G4LogicalVolume(muon_solid_ref, air_, "MUONS_ref");
  // new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic_ref,
  //                             "MUONS_ref", lab_logic_, false, 0, false);

  G4Box* muon_solid =
    new G4Box("MUONS", xMuons, yMuons, zMuons);
  G4LogicalVolume*  muon_logic = new G4LogicalVolume(muon_solid, air_, "MUONS");
  new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic,
		    "MUONS", lab_logic_, false, 0, false);

   // visualization
  G4VisAttributes muon_col = nexus::Red();
  muon_col.SetForceSolid(true);
  muon_logic->SetVisAttributes(muon_col);
}


void Next1EL::BuildExtScintillator()
{
  G4double vessel_total_diam = vessel_diam_ + 2.*vessel_thickn_;
  // rotation of the source-port
  G4RotationMatrix rot;
  rot.rotateY(-pi/2.);
  rot.rotateZ(sideport_angle_);

  sideNa_pos_ = G4ThreeVector((-vessel_total_diam/2. - sideport_length_ -
			sideport_flange_thickn_)
		       * cos(sideport_angle_),
			(-vessel_total_diam/2. - sideport_length_ -
			 sideport_flange_thickn_)
		       * sin(sideport_angle_),
		       sideport_posz_);

///The source itself
  G4double source_thick = .1*mm;
  G4double source_diam = 3.*mm;
  G4Tubs* source_solid =
    new G4Tubs("SOURCE", 0., source_diam/2., source_thick/2., 0., twopi);
  G4Material* sodium22_mat =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Na");
  sodium22_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  G4LogicalVolume* source_logic =
    new G4LogicalVolume(source_solid, sodium22_mat, "SOURCE");

  G4ThreeVector pos_source =
    G4ThreeVector( sideNa_pos_.getX()-source_thick/2.*cos(sideport_angle_),
		   sideNa_pos_.getY()-source_thick/2.*sin(sideport_angle_),
		   sideNa_pos_.getZ());

  new G4PVPlacement(G4Transform3D(rot, pos_source), source_logic, "SOURCE",
		    lab_logic_, false, 0, false);
  G4VisAttributes source_col = nexus::LightGreen();
  source_col.SetForceSolid(true);
  source_logic->SetVisAttributes(source_col);

   G4RotationMatrix* rot2 = new  G4RotationMatrix();
   rot2->rotateY(-pi/2.);
   rot2->rotateZ(sideport_angle_);

   cps_ =
     new CylinderPointSamplerLegacy(source_diam/2., source_thick, 0., 0., pos_source, rot2);


  ///Plastic support
  G4double support_thick = 6.2*mm;
  G4double support_diam = 25.33*mm;
  G4Tubs* support_solid =
    new G4Tubs("SUPPORT", 0., support_diam/2., support_thick/2., 0., twopi);
  G4LogicalVolume* support_logic =
    new G4LogicalVolume(support_solid, plastic_, "SUPPORT");
  G4ThreeVector pos_support =
    G4ThreeVector(pos_source.getX()-(support_thick+source_thick)/2.*cos(sideport_angle_),
		  pos_source.getY()-(support_thick+source_thick)/2.*sin(sideport_angle_),
		  pos_source.getZ());
  new G4PVPlacement(G4Transform3D(rot, pos_support), support_logic,
		    "SOURCE_SUPPORT",  lab_logic_, false, 0, false);

  G4VisAttributes support_col = nexus::Red();
  support_col.SetForceSolid(true);
  support_logic->SetVisAttributes(support_col);


  /// NaI scintillator behind
  G4double dist_sc = 5.*cm;
  G4double radius = 44.5/2.*mm;
  G4double length = 38.7*mm;
  G4Tubs* sc_solid = new G4Tubs("NaI", 0., radius, length/2., 0., twopi);
  G4Material* mat =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_SODIUM_IODIDE");
  mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
  G4LogicalVolume* sc_logic = new G4LogicalVolume(sc_solid, mat, "NaI");
  sc_logic->SetUserLimits(new G4UserLimits(1.*mm));

  // G4ThreeVector pos_scint =
  //   G4ThreeVector(pos_al.getX()-(al_thick/2.+dist_sc)*cos(sideport_angle_),
  // 		  pos_al.getY()-(al_thick/2.+dist_sc)*sin(sideport_angle_),
  // 		  sideNa.getZ());
  G4ThreeVector pos_scint =
    G4ThreeVector(pos_support.getX() -
		  (support_thick/2.+dist_sc+length/2.)*cos(sideport_angle_),
		  pos_support .getY()-(support_thick/2.+dist_sc+length/2.)*sin(sideport_angle_),
		  sideNa_pos_.getZ());
  new G4PVPlacement(G4Transform3D(rot, pos_scint), sc_logic, "NaI",
		    lab_logic_, false, 0, false);
  G4VisAttributes naI_col = nexus::Blue();
  naI_col.SetForceSolid(true);
  sc_logic->SetVisAttributes(naI_col);


  // NaI is defined as an ionization sensitive volume.
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4String detname = "/NEXT1/SCINT";
  IonizationSD* ionisd = new IonizationSD(detname);
  ionisd->IncludeInTotalEnergyDeposit(false);
  sdmgr->AddNewDetector(ionisd);
  sc_logic->SetSensitiveDetector(ionisd);


}



void Next1EL::BuildVessel()
{
  // CAN /////////////////////////////////////////////////////////////
  // Stainless steel (grade 304L) cylinder. CF-300.
  // A CF-60 half-nipple on the side accommodates a window for
  // calibration sources.

  G4double vessel_total_diam = vessel_diam_ + 2.*vessel_thickn_;

  // To avoid overlaps between volumes, the can solid volume
  // is the result of the union of the port and the CF-300 cylinder.

  G4Tubs* can_solid = new G4Tubs("VESSEL", 0., vessel_total_diam/2.,
				 vessel_length_/2., 0, twopi);

  G4Tubs* sideport_solid =
    new G4Tubs("VESSEL", 0., (sideport_diam_/2. + sideport_thickn_),
	       sideport_length_, 0, twopi);

  // rotation matrix for the side source port
  G4RotationMatrix rotport;
  rotport.rotateY(-pi/2.);
  rotport.rotateZ(sideport_angle_);

  // position of the side source-port
  G4ThreeVector posport(-vessel_total_diam/2. * cos(sideport_angle_),
			-vessel_total_diam/2. * sin(sideport_angle_),
			sideport_posz_);

  G4UnionSolid* vessel_solid =
    new G4UnionSolid("VESSEL", can_solid, sideport_solid,
		     G4Transform3D(rotport, posport));

  G4LogicalVolume* vessel_logic =
    new G4LogicalVolume(vessel_solid, steel_, "VESSEL");

  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_logic,
		    "VESSEL", lab_logic_, false, 0, false);


  // GAS /////////////////////////////////////////////////////////////
  // Gas filling the can.
  // Obviously, it has the same shape of the can, thus requiring
  // another union between solid volumes.

  G4Tubs* cyl_gas =
    new G4Tubs("GAS", 0., vessel_diam_/2., vessel_length_/2., 0, twopi);

  G4Tubs* sideport_gas =
    new G4Tubs("GAS", 0., sideport_diam_/2.,
	       (sideport_length_ + vessel_thickn_), 0, twopi);

  posport.setX(-vessel_diam_/2. * cos(sideport_angle_));
  posport.setY(-vessel_diam_/2. * sin(sideport_angle_));

  G4UnionSolid* gas_solid = new G4UnionSolid("GAS", cyl_gas, sideport_gas,
					     G4Transform3D(rotport, posport));

  gas_logic_ = new G4LogicalVolume(gas_solid, gxe_, "GAS");
  gas_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic_,
		    "GAS", vessel_logic, false, 0, false);


  // SIDE SOURCE-PORT ////////////////////////////////////////////////

  // FLANGE ................................................
  // CF flange closing the port.

  G4Tubs* sideport_flange_solid =
    new G4Tubs("SIDEPORT_FLANGE", sideport_tube_diam_/2.,
	       sideport_flange_diam_/2., sideport_flange_thickn_/2., 0, twopi);

  G4LogicalVolume* sideport_flange_logic =
    new G4LogicalVolume(sideport_flange_solid, steel_, "SIDEPORT_FLANGE");

  G4double radial_pos =
    -(vessel_total_diam/2. + sideport_length_ + sideport_flange_thickn_/2.);

  posport.setX(radial_pos * cos(sideport_angle_));
  posport.setY(radial_pos * sin(sideport_angle_));

  new G4PVPlacement(G4Transform3D(rotport, posport), sideport_flange_logic,
		    "SIDEPORT_FLANGE", lab_logic_, false, 0, false);
  G4VisAttributes * vis = new G4VisAttributes;
  vis->SetColor(0.5, 0.5, .5);
  vis->SetForceSolid(true);
  sideport_flange_logic->SetVisAttributes(vis);

  radial_pos = -(vessel_total_diam/2. + sideport_length_ + sideport_flange_thickn_);
  sideport_ext_position_.setX(radial_pos * cos(sideport_angle_));
  sideport_ext_position_.setY(radial_pos * sin(sideport_angle_));
  sideport_ext_position_.setZ(sideport_posz_);



  // TUBE ..................................................

  G4Tubs* sideport_tube_solid =
    new G4Tubs("SIDEPORT", 0., (sideport_tube_diam_/2.+sideport_tube_thickn_),
   	       sideport_tube_length_/2., 0, twopi);

  G4LogicalVolume* sideport_tube_logic =
    new G4LogicalVolume(sideport_tube_solid, steel_, "SIDEPORT");

  radial_pos =
    -(vessel_total_diam/2. + sideport_length_ - sideport_tube_length_/2.);
  posport.setX(radial_pos * cos(sideport_angle_));
  posport.setY(radial_pos * sin(sideport_angle_));

  sideport_position_ = posport;

  new G4PVPlacement(G4Transform3D(rotport, posport), sideport_tube_logic,
		    "SIDEPORT",  gas_logic_, false, 0, false);

  G4Tubs* sideport_tube_air_solid =
    new G4Tubs("SIDEPORT_AIR", 0., sideport_tube_diam_/2.,
	       (sideport_tube_length_ - sideport_tube_window_thickn_)/2.,
	       0, twopi);

  G4LogicalVolume* sideport_tube_air_logic =
    new G4LogicalVolume(sideport_tube_air_solid, air_, "SIDEPORT_AIR");

  new G4PVPlacement(0,G4ThreeVector(0.,0.,sideport_tube_window_thickn_/2.),
		    sideport_tube_air_logic, "SIDEPORT_AIR",
		    sideport_tube_logic, false, 0, false);


  // CATHODE ENDCAP //////////////////////////////////////////////////
  // Flat endcap, CF-300, no ports of interest for the simulation.

  G4Tubs* endcap_cathode_solid =
    new G4Tubs("ENDCAP_CATHODE", 0., endcap_diam_/2.,
	       endcap_thickn_/2., 0., twopi);

  G4LogicalVolume* endcap_cathode_logic =
    new G4LogicalVolume(endcap_cathode_solid, steel_, "ENDCAP_CATHODE");

  G4double posz = (vessel_length_ + endcap_thickn_) / 2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,-posz), endcap_cathode_logic,
  		    "ENDCAP_CATHODE", lab_logic_, false, 0, false);


  // ANODE ENDCAP ////////////////////////////////////////////////////
  // Flat endcap, CF-300, CF-16 port for calibration sources
  // Unlike the side-port, union between the endcap and port logical
  // volumes is not required. The endcap will have a central hole
  // where the port will be positioned.

  G4Tubs* endcap_anode_solid =
    new G4Tubs("ENDCAP_ANODE", (axialport_diam_/2. + axialport_thickn_),
	       endcap_diam_/2., endcap_thickn_/2., 0., twopi);

  G4LogicalVolume* endcap_anode_logic =
    new G4LogicalVolume(endcap_anode_solid, steel_, "ENDCAP_ANODE");

  new G4PVPlacement(0, G4ThreeVector(0., 0., posz), endcap_anode_logic,
		    "ENDCAP_ANODE", lab_logic_, false, 0, false);


  // AXIAL SOURCE-PORT ///////////////////////////////////////////////

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
   		    "AXIALPORT", lab_logic_, false, 0, false);

  G4Tubs* axialport_gas_solid = new G4Tubs("GAS", 0., axialport_diam_/2.,
					   axialport_total_length/2., 0, twopi);

  G4LogicalVolume* axialport_gas_logic =
    new G4LogicalVolume(axialport_gas_solid, gxe_, "GAS");

  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), axialport_gas_logic,
   		    "GAS", axialport_logic, false, 0, false);


  // FLANGE ................................................

  G4Tubs* axialport_flange_solid =
    new G4Tubs("AXIALPORT_FLANGE", axialport_tube_diam_/2.,
   	       axialport_flange_diam_/2., axialport_flange_thickn_/2., 0, twopi);

  G4LogicalVolume* axialport_flange_logic =
    new G4LogicalVolume(axialport_flange_solid, steel_, "AXIALPORT_FLANGE");

  posz = vessel_length_/2. + endcap_thickn_ +
    axialport_length_ + axialport_flange_thickn_/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_flange_logic,
   		    "AXIALPORT_FLANGE", lab_logic_, false, 0, false);

  // Store the position of the port so that it can be used in vertex generation
  posz = posz + axialport_flange_thickn_/2.;
  axialport_position_.set(0., 0., posz);


  // SOURCE TUBE ...........................................

  G4Tubs* axialport_tube_solid =
    new G4Tubs("AXIALPORT", 0., (axialport_tube_diam_/2.+axialport_tube_thickn_),
   	       axialport_tube_length_/2., 0, twopi);

  G4LogicalVolume* axialport_tube_logic =
    new G4LogicalVolume(axialport_tube_solid, steel_, "AXIALPORT");

  posz = (axialport_total_length - axialport_tube_length_) / 2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_tube_logic,
		    "AXIALPORT", axialport_gas_logic, false, 0, false);

  G4Tubs* axialport_tube_air_solid =
    new G4Tubs("AXIALPORT_AIR", 0., axialport_tube_diam_/2.,
   	       (axialport_tube_length_-axialport_tube_window_thickn_)/2.,
	       0, twopi);

  G4LogicalVolume* axialport_tube_air_logic =
    new G4LogicalVolume(axialport_tube_air_solid, air_, "AXIALPORT_AIR");

  new G4PVPlacement(0, G4ThreeVector(0,0,axialport_tube_window_thickn_/2.),
   		    axialport_tube_air_logic, "AXIALPORT_AIR",
		    axialport_tube_logic, false, 0, false);
}



void Next1EL::BuildFieldCage()
{
  // Store the position of the center of the fieldcage
  fieldcage_position_.
    set(0., 0., (vessel_length_/2.-fieldcage_length_/2.-fieldcage_displ_));

  // Position of the electrodes in the fieldcage
  anode_posz_   = fieldcage_length_/2. - elgap_ring_height_;
  gate_posz_    = anode_posz_ - elgap_length_;
  cathode_posz_ = gate_posz_ - active_length_;


  // ELECTROLUMINESCENCE GAP /////////////////////////////////////////

  // GATE AND ANODE (EL) RINGS .............................

  G4Tubs* elgap_ring_solid =
    new G4Tubs("EL_GAP_RING", elgap_ring_diam_/2.,
	       (elgap_ring_diam_/2. + elgap_ring_thickn_),
	       elgap_ring_height_/2., 0, twopi);

  G4LogicalVolume* elgap_ring_logic =
    new G4LogicalVolume(elgap_ring_solid, aluminum_, "EL_GAP_RING");

  G4double posz = fieldcage_length_/2. - elgap_ring_height_/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), elgap_ring_logic,
   		    "EL_GAP_RING", gas_logic_, false, 0, false);

  posz = posz - elgap_ring_height_ - elgap_length_;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), elgap_ring_logic,
   		    "EL_GAP_RING", gas_logic_, false, 1, false);


  // EL GAP ................................................

  G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., elgap_ring_diam_/2.,
   				   elgap_length_/2., 0, twopi);

  G4LogicalVolume* elgap_logic =
    new G4LogicalVolume(elgap_solid, gxe_, "EL_GAP");

  posz = fieldcage_length_/2. - elgap_ring_height_ - elgap_length_/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), elgap_logic,
		    "EL_GAP", gas_logic_, false, 0, false);


  // Store the position of the EL GAP wrt the WORLD system of reference
  elgap_position_.set(0.,0.,fieldcage_position_.z()+posz);

  // EL GRIDS

  G4double diel_thickn = .1*mm;

  G4Material* fgrid = materials::FakeDielectric(gxe_, "grid_mat");
  fgrid->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_, 303,
                                                           elgrid_transparency_,
                                                           diel_thickn, sc_yield_));
  G4Material* fgrid_gate = materials::FakeDielectric(gxe_, "grid_mat");
  fgrid_gate->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_, 303,
                                                                gate_transparency_,
                                                                diel_thickn, sc_yield_));

  G4Tubs* diel_grid =
    new G4Tubs("GRID", 0., elgap_ring_diam_/2., diel_thickn/2., 0, twopi);


  G4LogicalVolume* diel_grid_gate_logic =
    new G4LogicalVolume(diel_grid, fgrid_gate, "GRID_GATE");
  G4double pos1 = - elgap_length_/2. + diel_thickn/2.;
  new G4PVPlacement(0, G4ThreeVector(0.,0.,pos1), diel_grid_gate_logic, "GRID_GATE",
		    elgap_logic, false, 0, false);

  G4LogicalVolume* diel_grid_logic =
    new G4LogicalVolume(diel_grid, fgrid, "GRID");
  G4double pos2 = elgap_length_/2. - diel_thickn/2.;
  new G4PVPlacement(0, G4ThreeVector(0.,0.,pos2), diel_grid_logic, "GRID",
		    elgap_logic, false, 1, false);

  if (elfield_) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(elgap_position_.z()-elgap_length_/2.);
    el_field->SetAnodePosition(elgap_position_.z()+elgap_length_/2.);
    el_field->SetDriftVelocity(2.5*mm/microsecond);
    el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    el_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
    // el_field->SetLightYield(xgp.ELLightYield(24.8571*kilovolt/cm));//value for E that gives Y=1160 photons per ie- in normal conditions
    el_field->SetLightYield(XenonELLightYield(23.2857*kilovolt/cm, pressure_));
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(elgap_logic);
  }

  // ACTIVE VOLUME ///////////////////////////////////////////////////

  // Position of z planes
  G4double zplane[2] = {-active_length_/2., active_length_/2.};
  // Inner radius
  G4double rinner[2] = {0., 0.};
  // Outer radius
  G4double router[2] = {active_diam_/2., active_diam_/2.};

  G4Polyhedra* active_solid =
    new G4Polyhedra("ACTIVE", 0., twopi, 6, 2, zplane, rinner, router);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, gxe_, "ACTIVE");

  posz = fieldcage_length_/2. - elgap_ring_height_ -
    elgap_length_ - active_length_/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), active_logic,
		    "ACTIVE", gas_logic_, false, 0, false);

  // Store the position of the active volume with respect to the
  // WORLD system of reference
  active_position_.set(0.,0.,fieldcage_position_.z()+posz);

  hexrnd_ = new HexagonPointSampler(active_diam_/2., active_length_, 0.,
				    active_position_);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

  // Set the volume as an ionization sensitive detector
  G4String det_name = "/NEXT1/ACTIVE";
  IonizationSD* ionisd = new IonizationSD(det_name);
  active_logic->SetSensitiveDetector(ionisd);
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  //Define a drift field for this volume
  UniformElectricDriftField* field = new UniformElectricDriftField();
  field->SetCathodePosition(active_position_.z() - active_length_/2.);
  field->SetAnodePosition(elgap_position_.z()-elgap_length_/2.);
  field->SetDriftVelocity(1.*mm/microsecond);
  field->SetTransverseDiffusion(1.*mm/sqrt(cm));
  field->SetLongitudinalDiffusion(.3*mm/sqrt(cm));
  field->SetLifetime(e_lifetime_);
  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);

  active_logic->SetVisAttributes(G4VisAttributes::GetInvisible());


  // LIGHT TUBE //////////////////////////////////////////////////////
  // Hexagonal prism of teflon used as light tube. It transports the
  // EL light from anode to cathode improving the light collection
  // efficiency of the energy plane.

  // UPPER PART ............................................

  zplane[0] = -ltube_up_length_/2.; zplane[1] = ltube_up_length_/2.;
  rinner[0] = ltube_diam_/2.; rinner[1] = rinner[0];
  router[0] = ltube_diam_/2. + ltube_thickn_; router[1] = router[0];

  G4Polyhedra* ltube_up_solid =
    new G4Polyhedra("LIGHT_TUBE_UP", 0., twopi, 6, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_up_logic =
    new G4LogicalVolume(ltube_up_solid, teflon_, "LIGHT_TUBE_UP");

  posz = (gate_posz_ + cathode_posz_) / 2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), ltube_up_logic,
   		    "LIGHT_TUBE_UP", gas_logic_, false, 0, false);

  // TPB coating
  if (tpb_coating_) {
    G4double rinner_tpb[2];
    G4double router_tpb[2];
    rinner_tpb[0] = ltube_diam_/2.; rinner_tpb[1] = rinner_tpb[0];
    router_tpb[0] = ltube_diam_/2. + tpb_thickn_; router_tpb[1] = router_tpb[0];

    G4Polyhedra* ltube_tpb_solid =
      new G4Polyhedra("LIGHT_TUBE_TPB", 0., twopi, 6, 2, zplane,
		      rinner_tpb, router_tpb);
    G4LogicalVolume* ltube_tpb_logic =
      new G4LogicalVolume(ltube_tpb_solid, tpb_, "LIGHT_TUBE_TPB");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ltube_tpb_logic,
		      "LIGHT_TUBE_TPB", ltube_up_logic, false, 0, false);

    G4VisAttributes * visattrib_red = new G4VisAttributes;
    visattrib_red->SetColor(1., 0., 0.);
    ltube_tpb_logic->SetVisAttributes(visattrib_red);
  }

  // BOTTOM PART ...........................................

  zplane[0] = -ltube_bt_length_/2.; zplane[1] = ltube_bt_length_/2.;

  G4Polyhedra* ltube_bt_solid =
    new G4Polyhedra("LIGHT_TUBE_BOTTOM", 0., twopi, 6, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_bt_logic =
    new G4LogicalVolume(ltube_bt_solid, teflon_, "LIGHT_TUBE_BOTTOM");

  posz = fieldcage_length_/2. - elgap_ring_height_ - elgap_length_ -
    active_length_ - ltube_gap_ - ltube_bt_length_/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), ltube_bt_logic,
   		    "LIGHT_TUBE_BOTTOM", gas_logic_, false, 0, false);


  // FIELD SHAPING RINGS /////////////////////////////////////////////

  // G4Tubs* ring_solid = new G4Tubs("FIELD_RING", ring_diam_/2.,
  //  				  (ring_diam_/2.+ring_thickn_),
  // 				  ring_height_/2., 0, twopi);

  // G4LogicalVolume* ring_logic =
  //   new G4LogicalVolume(ring_solid, aluminum_, "FIELD_RING");


  // // DRIFT REGION ................................

  // G4int num_rings = 19;

  // posz = fieldcage_length_/2. - 2.*elgap_ring_height_
  //   - elgap_length_ - 5.*mm - ring_height_/2.;

  // for (G4int i=0; i<num_rings; i++) {

  //   new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
  //     		      "FIELD_RING", fieldcage_logic, false, i, true);

  //   posz = posz - ring_height_ - 5.1 * mm;
  // }


  // // BUFFER ......................................

  // posz = fieldcage_length_/2. - ring_height_ - elgap_length_
  //   - ltube_gap_ - ltube_up_length_ - ltube_gap_ - 10.*mm - ring_height_/2.;

  // for (G4int i=19; i<23; i++) {

  //    new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
  //     		      "FIELD_RING", fieldcage_logic, false, i, true);

  //   posz = posz - ring_height_ - 10. * mm;
  // }


  // CATHODE .....................................

  diel_thickn = 1. * mm;
  G4double transparency = 0.98;

  G4Material* fcathode = materials::FakeDielectric(gxe_, "cathode_mat");
  fcathode->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_, 303,
  									transparency, diel_thickn, sc_yield_, e_lifetime_));

  G4Tubs* diel_cathd =
    new G4Tubs("CATHODE", 0., elgap_ring_diam_/2., diel_thickn/2., 0, twopi);

  G4LogicalVolume* diel_cathd_logic =
    new G4LogicalVolume(diel_cathd, fcathode, "CATHODE");

  posz = cathode_posz_ - diel_thickn/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), diel_cathd_logic, "CATHODE",
		    gas_logic_, false, 0, false);


  // SUPPORT BARS ////////////////////////////////////////////////////

  G4double bar_length = fieldcage_length_ - 2.*elgap_ring_height_ - elgap_length_;

  G4Box* bar_base = new G4Box("SUPPORT_BAR", bar_thickn_/2.,
			 bar_width_/2., bar_length/2.);

  G4Box* addon = new G4Box("SUPPORT_BAR", bar_thickn_/2.,
			   bar_width_/2., bar_addon_length_/2.);

  G4UnionSolid* bar_solid =
    new G4UnionSolid("SUPPORT_BAR", bar_base, addon, 0,
		     G4ThreeVector(bar_thickn_, 0., (bar_length - bar_addon_length_)/2.));

  G4Material* peek_mat = materials::PEEK();
  peek_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4LogicalVolume* bar_logic =
    new G4LogicalVolume(bar_solid, peek_mat, "SUPPORT_BAR");


  G4double pos_rad = ring_diam_/2. + ring_thickn_ + bar_thickn_/2.;
  posz = fieldcage_length_/2. - 2.*elgap_ring_height_ -
    elgap_length_ - bar_length/2.;

  for (G4int i=0; i<6; i++) {

    G4RotationMatrix rotbar;
    rotbar.rotateZ(i*60.*deg);

    G4double xx = pos_rad * cos(i*60.*deg);
    G4double yy = pos_rad * sin(i*60.*deg);
    G4double zz = posz;

    new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz + fieldcage_position_.z())),
		      bar_logic, "SUPPORT_BAR", gas_logic_, false, i, false);
  }

  // OPTICAL SURFACES ////////////////////////////////////////////////
  G4OpticalSurface* ltubeup_opsur = new G4OpticalSurface("LIGHT_TUBE_UP");
  ltubeup_opsur->SetType(dielectric_metal);
  ltubeup_opsur->SetModel(unified);
  ltubeup_opsur->SetFinish(ground);
  ltubeup_opsur->SetSigmaAlpha(0.1);
  ltubeup_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

  G4OpticalSurface* ltubebt_opsur = new G4OpticalSurface("LIGHT_TUBE_BOTTOM");
  ltubebt_opsur->SetType(dielectric_metal);
  ltubebt_opsur->SetModel(unified);
  ltubebt_opsur->SetFinish(ground);
  ltubebt_opsur->SetSigmaAlpha(0.1);
  ltubebt_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

  new G4LogicalSkinSurface("LIGHT_TUBE_UP",     ltube_up_logic, ltubeup_opsur);
  new G4LogicalSkinSurface("LIGHT_TUBE_BOTTOM", ltube_bt_logic, ltubebt_opsur);
}



void Next1EL::BuildEnergyPlane()
{
  // PMT HOLDER //////////////////////////////////////////////////////
  // Teflon plate that holds the PMTs. Its front face has a hexagonal
  // cutout matching the size and shape of the light tube.

  G4Tubs* cyl_holder =
    new G4Tubs("PMT_HOLDER_CATHODE", 0., pmtholder_cath_diam_/2.,
  	       pmtholder_cath_height_/2., 0, twopi);

  G4double zplane[2] = {-pmtholder_cath_cutout_depth_/2.,
			pmtholder_cath_cutout_depth_/2.};
  G4double router[2] = {ltube_diam_/2., ltube_diam_/2.};
  G4double rinner[2] = {0., 0.};

  G4Polyhedra* cutout_holder =
    new G4Polyhedra("PMT_HOLDER_CATHODE", 0, twopi, 6, 2,
		    zplane, rinner, router);

  // The holder solid volume results from the subtraction of the two
  // previous volumes. In order to avoid the generation of a 'fake'
  // surface due to numerical precision loss, the two solids should
  // not have any common surface. Therefore, we add a slight security
  // margin to the translation of the cutout

  G4double secmargin = 0.5 * mm;
  G4double transl_z =
    pmtholder_cath_height_/2. + secmargin - pmtholder_cath_cutout_depth_/2.;

  G4SubtractionSolid* pmtholder_solid =
    new G4SubtractionSolid("PMT_HOLDER", cyl_holder, cutout_holder,
  			   0, G4ThreeVector(0, 0, transl_z));

  // The holder has holes were the PMTs are fitted.
  // (The placement is done below, in the same loop where we'll place
  // the PMTs.)

  const G4double hole_diam = 26. * mm;
  const G4double hole_depth = pmtholder_cath_height_;

  G4Tubs* hole_holder = new G4Tubs("PMT_HOLDER_CATHODE", 0., hole_diam/2.,
				   hole_depth/2., 0., twopi);

  // Z position of the PMT holder
  transl_z = fieldcage_position_.z() -
    (fieldcage_length_/2. + pmtholder_cath_displ_ + pmtholder_cath_height_/2.);

  // PHOTOMULTIPLIERS ////////////////////////////////////////////////

  // Load the geometry model of the PMT and get the pointer
  // to its logical volume
  PmtR7378A pmt;
  pmt.Construct();
  pmt_logic_ = pmt.GetLogicalVolume();

  // The PMTs are placed in the holder in a honeycomb arrangement.
  // We use the hexagon point sampler to calculate the positions of the
  // PMTs given the pitch.
  HexagonPointSampler hexsampler(ltube_diam_/2., 0., 0., G4ThreeVector(0.,0.,0.));
  hexsampler.TesselateWithFixedPitch(pmt_pitch_, pmt_positions_);

  // Loop over the vector of positions
  for (unsigned int i = 0; i<pmt_positions_.size(); i++) {

    // Make a hole for the PMT in the holder
    pmtholder_solid =
      new G4SubtractionSolid("PMT_HOLDER_CATHODE", pmtholder_solid,
			     hole_holder, 0, pmt_positions_[i]);

    // Place the PMT.
    // Notice that the PMT is not positioned inside the holder but
    // inside the gas. Therefore, the position (to be specific, the Z
    // coordinate) must be given in coordinates relative to the world.
    new G4PVPlacement(0, G4ThreeVector(pmt_positions_[i].x(),
  				       pmt_positions_[i].y(),
  				       transl_z-0.5*cm),
  		      pmt_logic_, "PMT", gas_logic_, false, i, false);
  }

  // Finish with the positioning of the PMT holder

  G4LogicalVolume* pmtholder_logic =
    new G4LogicalVolume(pmtholder_solid, teflon_, "PMT_HOLDER_CATHODE");

  new G4PVPlacement(0, G4ThreeVector(0.,0.,transl_z), pmtholder_logic,
  		    "PMT_HOLDER_CATHODE", gas_logic_, false, 0);



  // OPTICAL SURFACE ////////////////////////////////////////////////

  G4OpticalSurface* pmtholder_cath_opsur = new G4OpticalSurface("PMT_HOLDER_CATHODE");
  pmtholder_cath_opsur->SetType(dielectric_metal);
  pmtholder_cath_opsur->SetModel(unified);
  pmtholder_cath_opsur->SetFinish(ground);
  pmtholder_cath_opsur->SetSigmaAlpha(0.1);
  pmtholder_cath_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

  new G4LogicalSkinSurface("PMT_HOLDER_CATHODE", pmtholder_logic, pmtholder_cath_opsur);


  // PMT SHIELD //////////////////////////////////////////////////////

  G4double shield_thickn = 1. * mm;
  G4double transparency = 0.96;

  G4Material* fshield = materials::FakeDielectric(gxe_, "shield_mat");
  fshield->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_, 303,
  									transparency, shield_thickn, sc_yield_, e_lifetime_));

  G4Tubs* shield_solid =
    new G4Tubs("PMT_SHIELD", 0., elgap_ring_diam_/2., shield_thickn, 0, twopi);

  G4LogicalVolume* shield_logic =
    new G4LogicalVolume(shield_solid, fshield, "PMT_SHIELD");

  transl_z = fieldcage_position_.z() -
    (fieldcage_length_/2. + pmtholder_cath_displ_/2.);

  new G4PVPlacement(0, G4ThreeVector(0.,0.,transl_z), shield_logic,
  		    "PMT_SHIELD", gas_logic_, false, 0);

}


void Next1EL::BuildSiPMTrackingPlane()
{

   /* New Dice Board configuration */
  NextElDB db(8,8);
  db.Construct();

  G4LogicalVolume* db_logic = db.GetLogicalVolume();
  G4double db_xsize = db.GetDimensions().x();
  G4double db_ysize = db.GetDimensions().y();
  G4double db_zsize = db.GetDimensions().z();

  G4RotationMatrix* rotdb = new G4RotationMatrix();
  rotdb->rotateZ(pi);

  /// Distance of SiPM surface from the beginning of EL region
  G4double dist_el = 7.5*mm;
  G4double z = elgap_position_.z() + elgap_length_/2. + dist_el
    + db_zsize/2.;

   G4double gap = 1.*mm;
   G4int db_no = 1;

   for (G4int j=0; j<2; ++j) {
     G4double y = gap/2. + db_ysize/2. - j*(gap + db_xsize);
     for (G4int i=0; i<2; ++i) {
       G4double x =  - gap/2.- db_xsize/2. + i*(gap + db_xsize);
       if (j == 0) {
  	 new G4PVPlacement(rotdb, G4ThreeVector(x,y,z), db_logic,
  			   "DB", gas_logic_, false, db_no);
       } else {
  	 new G4PVPlacement(0, G4ThreeVector(x,y,z), db_logic,
  			   "DB", gas_logic_, false, db_no);
       }
       std::vector<std::pair<int, G4ThreeVector> > positions = db.GetPositions();
       for (unsigned int si=0; si<positions.size(); si++) {
  	 G4ThreeVector mypos = positions[si].second;
  	 G4ThreeVector mypos_rot = (*rotdb)*mypos;
  	 std::pair<int, G4ThreeVector> abs_pos;
  	 abs_pos.first = db_no*1000+positions[si].first ;
  	 if (j == 0) {
  	   abs_pos.second = G4ThreeVector(x+mypos_rot.getX(), y+mypos_rot.getY(), z+mypos.getZ());
  	 } else {
  	   abs_pos.second = G4ThreeVector(x+mypos.getX(),y+mypos.getY(),z+mypos.getZ());
  	 }
  	 absSiPMpos_.push_back(abs_pos);
       }
       db_no++;
     }
   }

  //  PrintAbsoluteSiPMPos();

   /* First Daughter Board configuration */
  // Next1ELDBO dbo44_geom(4,4);
  // Next1ELDBO dbo43_geom(4,3);
  // Next1ELDBO dbo42_geom(4,2);

  // G4LogicalVolume* dbo44_logic = dbo44_geom.GetLogicalVolume();
  // G4LogicalVolume* dbo43_logic = dbo43_geom.GetLogicalVolume();
  // G4LogicalVolume* dbo42_logic = dbo42_geom.GetLogicalVolume();

  // G4RotationMatrix* rotdbo = new G4RotationMatrix();
  // rotdbo->rotateY(pi);

  // G4double dbo44_xsize = dbo44_geom.GetDimensions().x();
  // G4double dbo44_ysize = dbo44_geom.GetDimensions().y();
  // G4double dbo43_xsize = dbo43_geom.GetDimensions().x();
  // G4double dbo43_ysize = dbo43_geom.GetDimensions().y();
  // G4double dbo42_xsize = dbo42_geom.GetDimensions().x();
  // G4double dbo42_ysize = dbo42_geom.GetDimensions().y();
  // // G4cout << "Size of dbo: " << dbo44_xsize << ", " << dbo44_ysize << G4endl;
  // G4double gap = 2.*mm;

  // G4int dbo_no = 0;

  // for (G4int i=0; i<4; i++) {
  //   G4double y = 3/2.*gap/2. + 3./2.*dbo44_ysize - i * (dbo44_ysize+gap);
  //   for (G4int j=0; j<3; j++) {
  //     G4double x = - dbo44_xsize - gap + j * (dbo44_xsize+gap);
  //     new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo44_logic,
  //  			"DBO44", gas_logic_, false, dbo_no, true);
  //     dbo_no++;
  //   }
  // }

  // G4double x = - 3./2.*dbo44_xsize - 2.*gap - dbo42_xsize/2.;
  // G4double y = gap + dbo44_ysize;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  //  			"DBO42", gas_logic_, false, 13, true);
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  // 		    "DBO42", gas_logic_, false, 14, true);
  // y = -y;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  // 		    "DBO42", gas_logic_, false, 15, true);
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  // 		    "DBO42", gas_logic_, false, 16, true);

  // x = - 3./2.*dbo44_xsize - 2.*gap - dbo43_xsize/2.;
  // y = 0;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo43_logic,
  // 		    "DBO43", gas_logic_, false, 17, true);
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo43_logic,
  // 		    "DBO43", gas_logic_, false, 18, true);



}

void Next1EL::BuildPMTTrackingPlane()
{
  // PMT HOLDER /////////////////////////////////////////////////////

  G4Tubs* cyl_holder =
    new G4Tubs("PMT_HOLDER_ANODE", 0., pmtholder_anode_diam_/2.,
	       pmtholder_anode_thickn_/2., 0., twopi);

  const G4double hole_diam  = 26. * mm;

  G4Tubs* hole_holder = new G4Tubs("PMT_HOLDER_ANODE", 0., hole_diam/2.,
				   pmtholder_anode_thickn_, 0., twopi);

  // Make the first hole to create the volume (the solid volume
  // has to exist already to use it in a subtraction, as we'll do
  // in the loop below).
  G4SubtractionSolid* pmtholder_solid =
    new G4SubtractionSolid("PMT_HOLDER_ANODE", cyl_holder, hole_holder,
			   0, pmt_positions_[0]);


  // PHOTOMULTIPLIERS ///////////////////////////////////////////////

  //Distance of PMT surface from the beginning of EL region
  G4double dist_el = 2.5*mm;
  // PMT position in gas
  PmtR7378A pmt;
  G4double pmt_length = pmt.Length();
  G4double posz = elgap_position_.z() + elgap_length_/2. + dist_el + pmt_length/2.;

  G4RotationMatrix rotpmt;
  rotpmt.rotateX(pi);


  // Since we had to make the central hole already to create the solid
  // volume, we place the central PMT outside the loop
  new G4PVPlacement(G4Transform3D(rotpmt,
				  G4ThreeVector(pmt_positions_[0].x(),
						pmt_positions_[0].y(),
						posz)),
		    pmt_logic_, "PMT", gas_logic_, false, 19);

  // the anode plane is rotated 0 degrees around the z axis
  G4RotationMatrix rotanode;
  rotanode.rotateZ(0.);

  std::vector<G4ThreeVector> pmt_pos_rot;
  pmt_pos_rot.push_back(G4ThreeVector(0., 0., 0.));

  for (unsigned int i=1; i<pmt_positions_.size(); i++){

    G4ThreeVector pos(pmt_positions_[i].x(),
			 pmt_positions_[i].y(),
			 0.);
    pos = rotanode*pos;
    pmt_pos_rot.push_back(pos);
  }

  // Loop over the PMT positions
  for (unsigned int i=1; i<pmt_pos_rot.size(); i++) {

    G4int pmt_no = i + pmt_positions_.size();

    pmtholder_solid =
      new G4SubtractionSolid("PMT_HOLDER_ANODE", pmtholder_solid,
			     hole_holder, 0, pmt_pos_rot[i]);

    new G4PVPlacement(G4Transform3D(rotpmt,
    				    G4ThreeVector(pmt_pos_rot[i].x(),
    						  pmt_pos_rot[i].y(),
    						  posz)),
    		      pmt_logic_, "PMT", gas_logic_, false, pmt_no);
  }

  G4LogicalVolume* pmtholder_logic =
    new G4LogicalVolume(pmtholder_solid, teflon_, "PMT_HOLDER_ANODE");

  new G4PVPlacement(0, G4ThreeVector(0,0,posz), pmtholder_logic,
   		    "PMT_HOLDER_ANODE", gas_logic_, false, 0);

  // OPTICAL SURFACE ////////////////////////////////////////////////

  G4OpticalSurface* pmtholder_anode_opsur = new G4OpticalSurface("PMT_HOLDER_ANODE");
  pmtholder_anode_opsur->SetType(dielectric_metal);
  pmtholder_anode_opsur->SetModel(unified);
  pmtholder_anode_opsur->SetFinish(ground);
  pmtholder_anode_opsur->SetSigmaAlpha(0.1);
  pmtholder_anode_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

  new G4LogicalSkinSurface("PMT_HOLDER_ANODE", pmtholder_logic, pmtholder_anode_opsur);
}

G4ThreeVector Next1EL::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0., 0., 0.);
  if (region == "CENTER") {
    vertex = active_position_;
  } else if (region == "SIDEPORT") {
    vertex = sideport_ext_position_;
  } else if (region == "AXIALPORT") {
    vertex = axialport_position_;
  } else if (region == "Na22LATERAL") {
    //    G4ThreeVector point = sideNa_pos_;
    vertex = sideNa_pos_;
  } else if (region == "ACTIVE") {
    vertex =  hexrnd_->GenerateVertex(INSIDE);
  } else if (region == "RESTRICTED") {
    vertex =  hexrnd_->GenerateVertex(PLANE);
    //  } else if (region == "FIXED_RADIUS") {
    //  G4ThreeVector point = hexrnd_->GenerateVertex(RADIUS, 10.);
    // G4cout <<  point.getX() << ", "
    // 	   <<  point.getY() << ", "
    // 	   <<  point.getZ() << G4endl;
    //    return  point;
  } else if (region == "AD_HOC"){
    vertex =  specific_vertex_;
  } else if (region == "EL_TABLE") {
      idx_table_++;
      if(idx_table_>=table_vertices_.size()){
    	G4cout<<"[Next1EL] Aborting the run, last event reached ..."<<G4endl;
    	G4RunManager::GetRunManager()->AbortRun();
      }
      if(idx_table_<=table_vertices_.size()){
    	vertex =  table_vertices_[idx_table_-1];
      }
  } else if (region == "MUONS") {
    //generate muons sampling the plane
    vertex = muons_sampling_->GenerateVertex("INSIDE");

  } else {
      G4Exception("[Next1EL]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

  return vertex;


}

void Next1EL::PrintAbsoluteSiPMPos()
{
  G4cout << "----- Absolute position of SiPMs in gas volume -----" << G4endl;
  G4cout <<  G4endl;
  for (unsigned int i=0; i<absSiPMpos_.size(); i++) {
    std::pair<int, G4ThreeVector> abs_pos = absSiPMpos_[i];
    G4cout << "ID number: " << absSiPMpos_[i].first << ", position: "
    	   << absSiPMpos_[i].second.getX() << ", "
    	   << absSiPMpos_[i].second.getY() << ", "
    	   << absSiPMpos_[i].second.getZ()  << G4endl;
  }
  G4cout <<  G4endl;
  G4cout << "---------------------------------------------------" << G4endl;
}

void Next1EL::CalculateELTableVertices(G4double radius,
				       G4double binning, G4double z)
{
    G4ThreeVector position;

    ///Squared pitch
    position[2] = z;

    G4int imax = floor(2*radius/binning);

    // const G4double a = 92.38;
    // const G4double b = 46.19;
    // const G4double c = 80.;

    for (int i=0; i<imax; i++){
      position[0] = -radius + i*binning;
      for (int j=0; j<imax; j++){
	position[1] = -radius + j*binning;
	//	G4cout << position[0] << ", " << position[1] << G4endl;
	if (sqrt(position[0]*position[0]+position[1]*position[1])< radius){
	  table_vertices_.push_back(position);

	} else {
	  continue;
	}
      }
    }
}
