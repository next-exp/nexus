// ----------------------------------------------------------------------------
// nexus | Next100Ics.cc
//
// Inner copper shielding of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100Ics.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "CylinderPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>


namespace nexus {

  using namespace CLHEP;

  Next100Ics::Next100Ics(G4double ics_ep_lip_width):
    GeometryBase(),
    in_rad_   (55.465 * cm),
    thickness_(12.0   * cm),
    ics_ep_lip_width_ (ics_ep_lip_width),
    visibility_ (0)
  {

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    msg_->DeclareProperty("ics_vis", visibility_, "ICS Visibility");

  }

  void Next100Ics::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }


  void Next100Ics::Construct()
  {
    // This visualization of this geometry takes a long time, because of
    // the subtraction of volumes. If needed, the user can comment by hand
    // one of the two subtractions and visualize the other.

    // (Full length of copper bars) - (copper simulated in TP_COPPER_PLATE)
    G4double length = 1468. * mm;
    // Defined for G4UnionSolids to ensure a common volume within the two
    // joined solids and for G4SubtractionSolids to ensure surface subtraction

    G4double offset = 1.* mm;

    // ICS
    G4double ics_z_pos = GetCoordOrigin().z() + length/2. - gate_tp_distance_;

    G4Tubs* ics_body = new G4Tubs("ICS", in_rad_, in_rad_ + thickness_,
                                  length/2., 0.*deg, 360.*deg);
    G4SubtractionSolid* ics_solid;

    // Port holes
    G4double port_hole_rad = 9. * mm;
    G4double port_x = (in_rad_ + thickness_/2.)/sqrt(2.);
    G4double port_y = port_x;

    G4RotationMatrix* port_a_Rot = new G4RotationMatrix;
    port_a_Rot->rotateX( 90. * deg);
    port_a_Rot->rotateY(-45. * deg);

    G4RotationMatrix* port_b_Rot = new G4RotationMatrix;
    port_b_Rot->rotateX( 90. * deg);
    port_b_Rot->rotateY( 45. * deg);

    G4Tubs* port_hole_solid =
      new G4Tubs("PORT_HOLE", 0., port_hole_rad,
                 (thickness_ + offset)/2., 0.*deg, 360.*deg);


   ics_solid =
      new G4SubtractionSolid("ICS", ics_body, port_hole_solid, port_a_Rot,
                             G4ThreeVector(port_x, port_y, port_z_1a_-ics_z_pos));

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, port_hole_solid, port_a_Rot,
                             G4ThreeVector(port_x, port_y, port_z_2a_-ics_z_pos));

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, port_hole_solid, port_b_Rot,
                             G4ThreeVector(-port_x, port_y, port_z_1b_-ics_z_pos));

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, port_hole_solid, port_b_Rot,
                             G4ThreeVector(-port_x, port_y, port_z_2b_-ics_z_pos));

    /// Upper holes
    // z distances measured with respect to TP plate, ie the start of ICS
    G4double upp_hole_1_rad = 31. * mm;
    G4double upp_hole_2_rad = 71. * mm;
    G4double upp_hole_1_z = 313. * mm;
    G4double upp_hole_2_z = upp_hole_1_z + 906.5 * mm;

    G4RotationMatrix* port_upp_Rot = new G4RotationMatrix;
    port_upp_Rot->rotateX( 90. * deg);

    G4Tubs* upp_hole_solid_1 =
      new G4Tubs("UPP_HOLE", 0., upp_hole_1_rad,
                 (thickness_ + offset)/2., 0.*deg, 360.*deg);

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, upp_hole_solid_1,
                             port_upp_Rot,
                             G4ThreeVector(0, (in_rad_ + thickness_/2.), upp_hole_1_z-length/2.));

    G4Tubs* upp_hole_solid_2 =
      new G4Tubs("UPP_HOLE", 0., upp_hole_2_rad, (thickness_ + offset)/2., 0.*deg, 360.*deg);

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, upp_hole_solid_2, port_upp_Rot,
                             G4ThreeVector(0, (in_rad_ + thickness_/2.), upp_hole_2_z-length/2.));

    /// Lateral holes (also known as feedthrough holes)
    G4double lat_hole_rad = upp_hole_1_rad;
    G4double lat_hole_z_1   = 55.5 * mm;
    G4double lat_hole_z_2   = 33.0 * mm;

    G4Tubs* lat_hole_solid =
      new G4Tubs("LAT_HOLE", 0., lat_hole_rad,
                 (thickness_ + offset)/2., 0.*deg, 360.*deg);

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, lat_hole_solid,
                             port_a_Rot, G4ThreeVector(port_x, port_y, lat_hole_z_1-length/2.));

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, lat_hole_solid,
                             port_b_Rot, G4ThreeVector(-port_x, port_y, lat_hole_z_2-length/2.));


    /// ICS step at the TP end.
    // This avoids overlap with sipm boards at high radius
    G4double step_width = 31. * mm; // step dim in y
    G4double step_length = 52 * mm; // step dim in z
    G4Tubs* tp_step_solid =
      new G4Tubs("TP_STEP", in_rad_ - offset, in_rad_ + step_width,
                 (step_length + offset)/2., 0.*deg, 360.*deg);

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, tp_step_solid, 0,
                             G4ThreeVector(0., 0., -length/2. + (step_length - offset)/2.));

    /// ICS lip at the EP end.
    // It is necessary to avoid clashing with the internal part
    // of the vessel flange.
    G4double lip_height = 47.85 * mm; // lip length in the y dimension
    G4Tubs* ep_lip_solid =
      new G4Tubs("EP_LIP", in_rad_ + lip_height, in_rad_ + thickness_ + offset,
                 (ics_ep_lip_width_ + offset)/2., 0.*deg, 360.*deg);

    ics_solid =
      new G4SubtractionSolid("ICS", ics_solid, ep_lip_solid, 0,
                             G4ThreeVector(0., 0., length/2. - (ics_ep_lip_width_ - offset)/2.));

    G4LogicalVolume* ics_logic =
      new G4LogicalVolume(ics_solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");

    new G4PVPlacement(0, G4ThreeVector(0., 0., ics_z_pos), ics_logic,
                      "ICS", mother_logic_, false, 0, false);


    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes copper_col = nexus::CopperBrownAlpha();
      copper_col.SetForceSolid(true);
      ics_logic->SetVisAttributes(copper_col);
    }
    else {
      ics_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // VERTEX GENERATOR
    ics_gen_ =
      new CylinderPointSampler(in_rad_, in_rad_ + thickness_, length/2.,
                               0.*deg, 360.*deg,
                               0, G4ThreeVector(0., 0., ics_z_pos));
  }


  Next100Ics::~Next100Ics()
  {
    delete ics_gen_;
  }


  G4ThreeVector Next100Ics::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region=="ICS"){
      G4VPhysicalVolume* VertexVolume;
      do {
        vertex = ics_gen_->GenerateVertex(VOLUME);

        G4ThreeVector glob_vtx(vertex);
        glob_vtx = glob_vtx - GetCoordOrigin();
        VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "ICS");
    }

    return vertex;
  }


  void Next100Ics::SetPortZpositions(G4double port_positions[])
  {
    port_z_1a_ = port_positions[0];
    port_z_2a_ = port_positions[1];
    port_z_1b_ = port_positions[2];
    port_z_2b_ = port_positions[3];
  }

} //end namespace nexus
