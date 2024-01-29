// -----------------------------------------------------------------------------
// nexus | NextNewPedestal.cc
//
// Table supporting the NEXT-WHITE vessel.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewPedestal.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "BoxPointSamplerLegacy.h"

#include <G4Material.hh>
#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewPedestal::NextNewPedestal():

    GeometryBase(),

    // Body dimensions
    table_x_ (1200. *mm),
    table_y_ (15. *mm),
    table_z_ (2500. *mm),
    y_pos_(-600. *mm),
    visibility_(1)

  {

   /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/",
                                  "Control commands of geometry NextNew.");
    msg_->DeclareProperty("table_vis", visibility_, "NEW pedestal visibility");

  }

  void NextNewPedestal::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewPedestal::Construct()
  {
    ////// SUPPORT TABLE  ///////////
    G4Box* table_solid =
      new G4Box("TABLE", table_x_/2., table_y_/2., table_z_/2.);

    G4Material* steel_316_Ti = materials::Steel316Ti();
    steel_316_Ti->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* table_logic = new G4LogicalVolume(table_solid,
						       steel_316_Ti,
						       "TABLE");
    new G4PVPlacement(0, G4ThreeVector(0.,y_pos_,0.), table_logic, "TABLE", mother_logic_, false, 0,false);

    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes steel_col = nexus::LightGrey();
      steel_col.SetForceSolid(true);
      table_logic->SetVisAttributes(steel_col);
    }
    else {
      table_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    // VERTEX GENERATORS   //////////
    table_gen_ = new BoxPointSamplerLegacy(table_x_,table_y_,table_z_,0.,
                                           G4ThreeVector(0.,y_pos_,0.), 0);

    // Calculating some probs
    // G4double table_vol = table_solid->GetCubicVolume();
    // std::cout<<"TABLE VOLUME:\t"<<table_vol<<std::endl;
  }

  NextNewPedestal::~NextNewPedestal()
  {
    delete table_gen_;
  }

  G4ThreeVector NextNewPedestal::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "PEDESTAL_BOARD") {
      vertex = table_gen_->GenerateVertex("INSIDE");
    }
    else {
      G4Exception("[NextNewPedestal]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }
    return vertex;
  }

  void NextNewPedestal::SetPosition(G4double pos)
  {
    y_pos_ = pos;
  }

  G4ThreeVector NextNewPedestal::GetDimensions() const
  {
    return G4ThreeVector(table_x_, table_y_, table_z_);
  }


} //end namespace nexus
