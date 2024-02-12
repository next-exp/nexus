// ----------------------------------------------------------------------------
// nexus | Honeycomb.cc
//
// Support structure to the EP copper plate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Honeycomb.h"
#include "HoneycombBeam.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "CylinderPointSampler2020.h"

#include <G4LogicalVolume.hh>
#include <G4RotationMatrix.hh>
#include <G4MultiUnion.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Trd.hh>
#include <G4TransportationManager.hh>

namespace nexus {

  using namespace CLHEP;

  Honeycomb::Honeycomb(): GeometryBase(),
                          angle_(pi / 6.),
                          beam_dist_(102.25 * mm),
                          beam_thickn_(6 * mm)
  {
    compl_angle_ = pi/2. - angle_;

    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ =
      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  }

  Honeycomb::~Honeycomb()
  {
  }

  void Honeycomb::Construct()
  {
    G4double shorter_height = 117 * mm;
    G4double longer_height  = 130 * mm;
    G4double longer_length  = 1210.6*mm;

    HoneycombBeam* short_beam =
      new HoneycombBeam(beam_thickn_, 1085.9*mm, 184*mm, shorter_height);
    HoneycombBeam* medium_beam =
      new HoneycombBeam(beam_thickn_, 1170.2*mm, 273.5*mm, longer_height);
    HoneycombBeam* long_beam =
      new HoneycombBeam(beam_thickn_, longer_length, 407.4*mm, longer_height);

    G4double short_displ = (longer_height-shorter_height)/2.;

    short_beam->Construct();
    medium_beam->Construct();
    long_beam->Construct();


    G4MultiUnion* struct_solid = new G4MultiUnion("HONEYCOMB");

    G4RotationMatrix rot;
    rot.rotateX(3*pi/2.);

    G4ThreeVector ini_pos =
      G4ThreeVector(-(beam_dist_+beam_thickn_)/2., 0., 0.);
    struct_solid->AddNode(long_beam->GetSolidVol(),
                          G4Transform3D(rot, ini_pos));

    G4RotationMatrix rot_left;
    rot_left.rotateX(3*pi/2.);
    rot_left.rotateY(-compl_angle_);
    G4RotationMatrix rot_right;
    rot_right.rotateX(3*pi/2.);
    rot_right.rotateY(-(pi/2.+angle_));

    G4double r_pos =
      1/2*beam_dist_+ 2*(beam_dist_+beam_thickn_) + 1/2*beam_thickn_;
    r_pos =  r_pos + (beam_dist_ + beam_thickn_)/2.*cos(compl_angle_) + 2.75*cm;
    G4double x = r_pos * sin(angle_);
    G4double z = r_pos * cos(angle_);

    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot_left,
                                        G4ThreeVector(x, short_displ, z)));
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot_right,
                                        G4ThreeVector(-x, short_displ, z)));
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot_left,
                                        G4ThreeVector(-x, short_displ, -z)));
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot_right,
                                        G4ThreeVector(x, short_displ, -z)));


    x = (r_pos-(beam_dist_+beam_thickn_)) * sin(angle_);
    z = (r_pos-(beam_dist_+beam_thickn_)) * cos(angle_);
    struct_solid->AddNode(medium_beam->GetSolidVol(),
                          G4Transform3D(rot_left, G4ThreeVector(x, 0., z)));
    struct_solid->AddNode(medium_beam->GetSolidVol(),
                          G4Transform3D(rot_left, G4ThreeVector(-x, 0., -z)));
    struct_solid->AddNode(medium_beam->GetSolidVol(),
                          G4Transform3D(rot_right, G4ThreeVector(x, 0., -z)));

    struct_solid->AddNode(medium_beam->GetSolidVol(),
                          G4Transform3D(rot_right, G4ThreeVector(-x, 0., z)));


    x = (r_pos-2*(beam_dist_+beam_thickn_)) * sin(angle_);
    z = (r_pos-2*(beam_dist_+beam_thickn_)) * cos(angle_);
    struct_solid->AddNode(long_beam->GetSolidVol(),
                          G4Transform3D(rot_left, G4ThreeVector(x, 0., z)));


    struct_solid->AddNode(long_beam->GetSolidVol(),
                          G4Transform3D(rot_left, G4ThreeVector(-x, 0., -z)));

    struct_solid->AddNode(long_beam->GetSolidVol(),
                          G4Transform3D(rot_right, G4ThreeVector(x, 0., -z)));
    struct_solid->AddNode(long_beam->GetSolidVol(),
                          G4Transform3D(rot_right, G4ThreeVector(-x, 0., z)));


    x = ini_pos.x() - (beam_dist_+beam_thickn_);
    struct_solid->AddNode(medium_beam->GetSolidVol(),
                          G4Transform3D(rot, G4ThreeVector(x, 0., 0.)));
    x = ini_pos.x() - 2*(beam_dist_+beam_thickn_);
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot,
                                        G4ThreeVector(x, short_displ, 0.)));
    x = ini_pos.x() + (beam_dist_+beam_thickn_);
    struct_solid->AddNode(long_beam->GetSolidVol(),
                          G4Transform3D(rot, G4ThreeVector(x, 0., 0.)));
    x = ini_pos.x() + 2*(beam_dist_+beam_thickn_);
    struct_solid->AddNode(medium_beam->GetSolidVol(),
                          G4Transform3D(rot, G4ThreeVector(x, 0., 0.)));
    x = ini_pos.x() + 3*(beam_dist_+beam_thickn_);
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot,
                                        G4ThreeVector(x, short_displ, 0.)));

    struct_solid->Voxelize();

    G4LogicalVolume* struct_logic =
      new G4LogicalVolume(struct_solid, materials::Steel(), "HONEYCOMB");

    G4RotationMatrix rot_placement;
    rot_placement.rotateZ(-pi/2);
    rot_placement.rotateY(-pi/2);

    G4double hc_posz = end_of_EP_copper_plate_z_ + longer_height/2.;
    new G4PVPlacement(G4Transform3D(rot_placement,
                                    G4ThreeVector(0., 0., hc_posz)),
                      struct_logic, "HONEYCOMB", mother_logic_,
                      false, 0, false);

    G4VisAttributes red_col = nexus::Red();
    red_col.SetForceSolid(true);
    struct_logic->SetVisAttributes(red_col);

    // Vertex generator
    gen_  =
      new CylinderPointSampler2020(0., longer_length/2., longer_height/2.,
                                   0., 360.*deg, 0,
                                   G4ThreeVector(0., 0., hc_posz));

  }

  G4ThreeVector Honeycomb::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0, 0, 0);

    if (region == "HONEYCOMB") {
      G4VPhysicalVolume* VertexVolume;
      do {
        vertex = gen_->GenerateVertex("VOLUME");
        G4ThreeVector glob_vtx(vertex);
        glob_vtx = glob_vtx - GetCoordOrigin();
        VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != region);
    }
    else {
      G4Exception("[Honeycomb]", "GenerateVertex()", FatalException,
     		  "Unknown vertex generation region!");
    }

    return vertex;
  }
}
