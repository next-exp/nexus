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

#include <G4LogicalVolume.hh>
#include <G4RotationMatrix.hh>
#include <G4MultiUnion.hh>
#include <G4VisAttributes.hh>


namespace nexus {
  
  using namespace CLHEP;
  
  Honeycomb::Honeycomb(): GeometryBase(), angle_(pi / 6.), beam_dist_(102.25 * mm),
                          beam_thickn_(6 * mm)
  {
  }
  
  Honeycomb::~Honeycomb()
  {
  }

  void Honeycomb::Construct()
  {
    HoneycombBeam* short_beam  = new HoneycombBeam(1085.9*mm, 117*mm, beam_thickn_);
    HoneycombBeam* medium_beam = new HoneycombBeam(1170.2*mm, 130*mm, beam_thickn_);
    HoneycombBeam* long_beam   = new HoneycombBeam(1210.6*mm, 130*mm, beam_thickn_);

    short_beam->Construct();
    medium_beam->Construct();
    long_beam->Construct();

     
    G4MultiUnion* struct_solid = new G4MultiUnion("HONEYCOMB");

    G4RotationMatrix rot;
    G4double long_hup_ypos = beam_dist_/2. + beam_thickn_;
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot, G4ThreeVector(0., long_hup_ypos, 0.)));

    G4RotationMatrix rot_left;
    rot_left.rotateY(pi / 6);
    G4double r_pos = 2.5*beam_thickn_+2.5*beam_dist_;
    G4double z = r_pos * cos(angle_);
    G4double x = r_pos * sin(angle_);
    struct_solid->AddNode(short_beam->GetSolidVol(),
                          G4Transform3D(rot_left, G4ThreeVector(x, 0, z)));

    struct_solid->Voxelize();

    G4LogicalVolume* struct_logic = new G4LogicalVolume(struct_solid, materials::Steel(),
                                                "UnionBeams");

    this->SetLogicalVolume(struct_logic);

    G4VisAttributes red_col = nexus::Red();
    red_col.SetForceSolid(true);
    struct_logic->SetVisAttributes(red_col);
    
  }

  G4ThreeVector Honeycomb::GenerateVertex(const G4String& region) const
  {
    return G4ThreeVector(0, 0, 0);
  }
}
