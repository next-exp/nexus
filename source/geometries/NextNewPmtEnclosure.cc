// -----------------------------------------------------------------------------
// nexus | NextNewPmtEnclosure.cc
//
// Pseudo-enclosure that describes the copper around the PMTs.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewPmtEnclosure.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "CylinderPointSamplerLegacy.h"
#include "Visibilities.h"
#include "PmtR11410.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <Randomize.hh>
#include <G4OpticalSurface.hh>

namespace nexus{

  NextNewPmtEnclosure::NextNewPmtEnclosure():
    GeometryBase(),
    // Enclosures dimensions
    enclosure_in_diam_ (80. * mm),
    enclosure_length_ (200.0 * mm),
    enclosure_thickness_ (6. * mm),
    enclosure_endcap_diam_ (119. * mm),
    enclosure_endcap_thickness_ (60. * mm),
    enclosure_window_diam_ (85. * mm),
    enclosure_window_thickness_ (6. * mm), //???
    enclosure_pad_thickness_ (1. * mm),//max 60  ??????
    //   enclosure_tpb_thickness_(1.*micrometer),
    pmt_base_diam_ (47. *mm),
    pmt_base_thickness_ (5. *mm),
    pmt_base_z_ (50. *mm), //distance from window
    visibility_(1)
  {
    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("enclosure_vis", visibility_, "Vessel Visibility");

    /// The PMT
    pmt_ = new PmtR11410();
  }


  NextNewPmtEnclosure::~NextNewPmtEnclosure()
  {
    delete enclosure_body_gen_;
    delete enclosure_flange_gen_;
    delete enclosure_cap_gen_;
    delete enclosure_window_gen_;
    delete enclosure_pad_gen_;
    delete pmt_base_gen_;
    delete enclosure_surf_gen_;
    delete pmt_;
    delete msg_;
  }


  void NextNewPmtEnclosure::Construct()
  {
    // MATERIALS ///////////////////////////////////////////

    G4Material* sapphire = materials::Sapphire();
    sapphire->SetMaterialPropertiesTable(opticalprops::Sapphire());

    G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(opticalprops::Vacuum());

    G4Material* optical_coupler = materials::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(opticalprops::OptCoupler());

    G4Material* tpb = materials::TPB();
    tpb->SetMaterialPropertiesTable(opticalprops::TPB());

    G4Material* pedot = materials::PEDOT();
    pedot->SetMaterialPropertiesTable(opticalprops::PEDOT());

    /////   ENCLOSURES  /////
    G4Tubs* enclosure_body =
      new G4Tubs("ENCLOSURE_BODY", 0.,
                 enclosure_in_diam_/2.+ enclosure_thickness_,
                 enclosure_length_/2., 0., twopi);
    G4Tubs* enclosure_endcap =
      new G4Tubs("ENCLOSURE_ENDCAP", 0., enclosure_endcap_diam_/2,
                 enclosure_endcap_thickness_/2, 0., twopi);
    G4ThreeVector transl(0., 0., -enclosure_length_/2 + enclosure_endcap_thickness_/2);
    G4UnionSolid* enclosure_solid =
      new G4UnionSolid("ENCLOSURE_SOLID", enclosure_body, enclosure_endcap, nullptr, transl);

    G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    copper->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* enclosure_logic =
      new G4LogicalVolume(enclosure_solid,
                          copper,
                          "ENCLOSURE");
    GeometryBase::SetLogicalVolume(enclosure_logic);

    // Filling the enclosure with vacuum
    G4double gas_diam = enclosure_in_diam_ ;
    G4double gas_length = enclosure_length_ - enclosure_endcap_thickness_/2 -
                          enclosure_window_thickness_ - enclosure_pad_thickness_;
    G4Tubs* enclosure_gas_solid =
      new G4Tubs("ENCLOSURE_GAS", 0., gas_diam/2., gas_length/2., 0., twopi);
    G4LogicalVolume* enclosure_gas_logic =
      new G4LogicalVolume(enclosure_gas_solid, vacuum, "ENCLOSURE_GAS");
    gas_pos_ = (enclosure_endcap_thickness_/2-enclosure_window_thickness_-
     	          enclosure_pad_thickness_)/2;
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,gas_pos_), enclosure_gas_logic,
                      "ENCLOSURE_GAS", enclosure_logic, false, 0, false);

    // Adding the sapphire window
    G4Tubs* enclosure_window_solid =
      new G4Tubs("ENCLOSURE_WINDOW", 0., enclosure_window_diam_/2.,
                 enclosure_window_thickness_/2., 0., twopi);
    G4LogicalVolume* enclosure_window_logic =
      new G4LogicalVolume(enclosure_window_solid, sapphire,
                          "ENCLOSURE_WINDOW");

    window_z_pos_ = enclosure_length_/2 - enclosure_window_thickness_/2.;
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,window_z_pos_),
                      enclosure_window_logic, "ENCLOSURE_WINDOW",
                      enclosure_logic, false, 0, false);

    // TPB COATING ON SAPPHIRE WINDOW //////////////////////

    G4double tpb_coating_thickn = 1.*micrometer;

    G4Tubs* tpb_coating_solid = new G4Tubs("ENCLOSURE_WINDOW_TPB",
                                           0., enclosure_window_diam_/2.,
                                           tpb_coating_thickn/2., 0., twopi);

    G4LogicalVolume* tpb_coating_logic =
      new G4LogicalVolume(tpb_coating_solid, tpb, "ENCLOSURE_WINDOW_TPB");

    G4double tpb_coating_zpos = enclosure_window_thickness_/2. - tpb_coating_thickn/2.;
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., tpb_coating_zpos),
                      tpb_coating_logic, "ENCLOSURE_WINDOW_TPB", enclosure_window_logic,
                      false, 0, false);

    G4OpticalSurface* tpb_coating_opsurf =
      new G4OpticalSurface("ENCLOSURE_WINDOW_TPB_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);
    new G4LogicalSkinSurface("ENCLOSURE_WINDOW_TPB_OPSURF",
                             tpb_coating_logic, tpb_coating_opsurf);

    // PEDOT COATING ON SAPPHIRE WINDOW ////////////////////

    G4double pedot_coating_thickn = 200.*nm;

    G4Tubs* pedot_coating_solid =
      new G4Tubs("ENCLOSURE_WINDOW_PEDOT", 0., enclosure_window_diam_/2.,
                 pedot_coating_thickn/2., 0., twopi);

    G4LogicalVolume* pedot_coating_logic =
      new G4LogicalVolume(pedot_coating_solid, pedot, "ENCLOSURE_WINDOW_PEDOT");

    G4double pedot_coating_zpos =
      enclosure_window_thickness_/2. - tpb_coating_thickn - pedot_coating_thickn/2.;
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,pedot_coating_zpos),
                      pedot_coating_logic, "ENCLOSURE_WINDOW_PEDOT", enclosure_window_logic,
                      false, 0, false);

    // Adding the optical pad
    G4Tubs* enclosure_pad_solid =
      new G4Tubs("OPTICAL_PAD", 0., enclosure_in_diam_/2.,
                 enclosure_pad_thickness_/2.,0.,twopi);
    G4LogicalVolume* enclosure_pad_logic =
      new G4LogicalVolume(enclosure_pad_solid, optical_coupler, "OPTICAL_PAD");
    G4double pad_z_pos =
      window_z_pos_-enclosure_window_thickness_/2.-enclosure_pad_thickness_/2.;
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,pad_z_pos),
                      enclosure_pad_logic,"OPTICAL_PAD",
                      enclosure_logic, false, 0, false);

    // Adding the PMT
    pmt_->SetSensorDepth(4);
    pmt_->Construct();
    G4LogicalVolume* pmt_logic = pmt_->GetLogicalVolume();
    G4double pmt_rel_z_pos = pmt_->GetRelPosition().z();
    pmt_z_pos_ = gas_length/2.- pmt_rel_z_pos;
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,pmt_z_pos_), pmt_logic,
                      "PMT", enclosure_gas_logic, false, 0, false);
    // Adding the PMT base
    G4Tubs* pmt_base_solid =
      new G4Tubs("PMT_BASE", 0., pmt_base_diam_/2., pmt_base_thickness_, 0.,twopi);
    
    G4Material* kapton = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    kapton->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* pmt_base_logic =
      new G4LogicalVolume(pmt_base_solid,
                          kapton,
                          "PMT_BASE");
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0., -pmt_base_z_),
                      pmt_base_logic, "PMT_BASE", enclosure_gas_logic, false, 0, false);


    /////  SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes copper_col = nexus::CopperBrown();
      enclosure_logic->SetVisAttributes(copper_col);
      G4VisAttributes sapphire_col = nexus::Lilla();
      sapphire_col.SetForceSolid(true);
      enclosure_window_logic->SetVisAttributes(sapphire_col);
      G4VisAttributes pad_col = nexus::LightGreen();
      pad_col.SetForceSolid(true);
      enclosure_pad_logic->SetVisAttributes(pad_col);
      G4VisAttributes base_col = nexus::Yellow();
      base_col.SetForceSolid(true);
      pmt_base_logic->SetVisAttributes(base_col);
    } else {
      enclosure_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      enclosure_window_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      enclosure_pad_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      pmt_base_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // VERTEX GENERATORS   //////////
    enclosure_body_gen_ =
      new CylinderPointSamplerLegacy(enclosure_in_diam_/2.,
                               enclosure_length_ - enclosure_endcap_thickness_,
                               enclosure_thickness_, 0.,
                               G4ThreeVector(0., 0., 0.), 0);
    enclosure_flange_gen_ =
      new CylinderPointSamplerLegacy(enclosure_in_diam_/2.,
                               enclosure_endcap_thickness_,
                               enclosure_endcap_diam_/2.-enclosure_in_diam_/2.
                               , 0., G4ThreeVector(0., 0., - enclosure_length_/2. +  enclosure_endcap_thickness_/2.));
    enclosure_cap_gen_ =
      new CylinderPointSamplerLegacy(0., enclosure_endcap_thickness_/2.,
                               enclosure_in_diam_/2., 0.,
                               G4ThreeVector (0., 0., - enclosure_length_/2. + enclosure_endcap_thickness_/4.));
    enclosure_window_gen_ =
      new CylinderPointSamplerLegacy(0., enclosure_window_thickness_,
                               enclosure_window_diam_/2., 0.,
                               G4ThreeVector (0., 0., enclosure_length_/2. - enclosure_window_thickness_/2.));
    enclosure_pad_gen_ =
      new CylinderPointSamplerLegacy(0., enclosure_pad_thickness_, enclosure_in_diam_/2., 0., G4ThreeVector(0.,0.,pad_z_pos));

    pmt_base_gen_ =
      new CylinderPointSamplerLegacy(0., pmt_base_thickness_, pmt_base_diam_/2., 0., G4ThreeVector(0.,0., -pmt_base_z_ +10.*mm ));//10mm fitting???

    enclosure_surf_gen_ =
      new CylinderPointSamplerLegacy(gas_diam/2., gas_length, 0., 0., G4ThreeVector (0., 0., gas_pos_));

    enclosure_cap_surf_gen_ =
      new CylinderPointSamplerLegacy(0., 0.1 * micrometer, gas_diam/2., 0.,
                               G4ThreeVector (0., 0., - gas_pos_/2. + 0.05 * micrometer));


    // Getting the enclosure body volume over total
    G4double body_vol =
      (enclosure_length_-enclosure_endcap_thickness_) * pi *
      ((enclosure_in_diam_/2.+enclosure_thickness_)*
       (enclosure_in_diam_/2.+enclosure_thickness_) -
       (enclosure_in_diam_/2.)*(enclosure_in_diam_/2.) );
    G4double flange_vol =
      enclosure_endcap_thickness_ * pi *
      ((enclosure_endcap_diam_/2.)*(enclosure_endcap_diam_/2.)-
       (enclosure_in_diam_/2.)*(enclosure_in_diam_/2.));
    G4double cap_vol =
      enclosure_endcap_thickness_/2 * pi *
      (enclosure_in_diam_/2.) * (enclosure_in_diam_/2.);
    G4double total_vol = body_vol + flange_vol + cap_vol;
    body_perc_ = body_vol / total_vol;
    flange_perc_ =  (flange_vol + body_vol) / total_vol;
    G4double enclosure_int_surf = 2. * pi * gas_diam/2. * gas_length;
    G4double enclosure_int_cap_surf = pi * gas_diam/2. * gas_diam/2.;
    G4double total_surf = enclosure_int_surf + enclosure_int_cap_surf;
    int_surf_perc_ = enclosure_int_surf / total_surf;
    int_cap_surf_perc_ = enclosure_int_cap_surf / total_surf;
  }


  G4ThreeVector NextNewPmtEnclosure::GetObjectCenter()
  {
    return G4ThreeVector(0., 0., enclosure_length_/2.);
  }


  G4double NextNewPmtEnclosure::GetWindowDiameter()
  {
    return enclosure_window_diam_;
  }


  G4ThreeVector NextNewPmtEnclosure::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    /// Enclosures bodies
    if (region == "ENCLOSURE_BODY") {
      G4double rand1 = G4UniformRand();
      // Generating in the cilindric part of the enclosure
      if (rand1 < body_perc_) {
	vertex = enclosure_body_gen_->GenerateVertex("BODY_VOL");
      }
      // Generating in the union/flange
      else if (rand1 < flange_perc_){
       	vertex = enclosure_flange_gen_->GenerateVertex("BODY_VOL");
      }
      // Generating in the rear cap of the enclosure
      else {
       	vertex = enclosure_cap_gen_->GenerateVertex("BODY_VOL");
      }

    }
    /// Enclosures windows
    else if (region == "ENCLOSURE_WINDOW") {
      vertex = enclosure_window_gen_->GenerateVertex("BODY_VOL");
      //z translation made in CylinderPointSampler
    }
    //Optical pad
    else if (region=="OPTICAL_PAD"){
      vertex =enclosure_pad_gen_->GenerateVertex("BODY_VOL");
      //z translation made in CylinderPointSampler
    }
    //PMT base
    else if (region=="PMT_BASE"){
      vertex =pmt_base_gen_->GenerateVertex("BODY_VOL");
      //z translation made in CylinderPointSampler
      //std::cout<<"vertx z  "<< vertex.z()<<std::endl;
    }
    //PMTs bodies
    else if (region == "PMT_BODY") {
      vertex = pmt_->GenerateVertex(region);
      vertex.setZ(vertex.z() + pmt_z_pos_ + gas_pos_);
    }
    // Internal surface of enclosure
    else if (region == "INT_ENCLOSURE_SURF") {
      G4double rand1 = G4UniformRand();
      if (rand1 < int_surf_perc_) {
        vertex = enclosure_surf_gen_->GenerateVertex("BODY_SURF");
      }
      else {
        vertex = enclosure_cap_surf_gen_->GenerateVertex("WHOLE_VOL");
      }
    }
    // External surface of PMT
    else if (region == "PMT_SURF") {
      vertex = pmt_->GenerateVertex(region);
      vertex.setZ(vertex.z() + pmt_z_pos_);
    }
    return vertex;
  }

}//end namespace nexus
