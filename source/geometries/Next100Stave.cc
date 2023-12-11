// ----------------------------------------------------------------------------
// nexus | Next100Stave.cc
//
// Geometry of the staves that support the field rings
// in the NEXT-100 field cage.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100Stave.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4UnionSolid.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

using namespace nexus;

Next100Stave::Next100Stave(G4double ring_drift_buffer_dist,
                           G4int num_drift_rings,
                           G4int num_buffer_rings):
  GeometryBase(),
  visibility_(0),
  num_drift_rings_ (num_drift_rings),
  num_buffer_rings_ (num_buffer_rings),
  ring_drift_buffer_dist_ (ring_drift_buffer_dist), // Distance between last ring of buffer and first ring of drift part

  holder_x_         (60. * mm),  // x dimension of the staves
  holder_long_y_    (9.1 * mm),  // y dim of the base of the ring staves
  holder_short_y_   (33.9 * mm), // y dim of the pieces added over the base of the ring staves

  buffer_ring_dist_ (48. * mm),
  buffer_long_length_ (241.3 * mm),
  buffer_short_length_ (37. * mm),  // Thickness of staves in buffer volume
  buffer_last_z_ (63.5 *mm),

  drift_ring_dist_  (24. * mm),
  drift_long_length_ (1170.2 * mm),
  drift_short_first_length_ (8.5 * mm), // Thickness of first stave in the active volume
  drift_short_length_ (13. * mm), // Thickness of staves in the active volume

  cathode_opening_  (15.5 * mm),
  cathode_long_y_ (28.15 * mm),
  cathode_long_length_ (61 * mm),
  cathode_short_length_ (22.75 * mm),

  overlap_ (0.001*mm) // defined to ensure a common volume within two joined solids
{
  /// Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
                                "Control commands of geometry Next100.");
  msg_->DeclareProperty("stave_vis", visibility_, "Stave Visibility");
}

Next100Stave::~Next100Stave()
{
}

void Next100Stave::Construct()
{
  G4Material* pe500 = materials::PE500();

  // BUFFER part.
  // The unions of volumes go from lower to higher z.
  G4Box* buffer_short_solid =
    new G4Box("BUFF_SHORT", holder_x_/2., holder_short_y_/2. + overlap_/2.,
              buffer_short_length_/2.);

  G4Box* buffer_long_solid =
    new G4Box("BUFF_LONG", holder_x_/2., holder_long_y_/2.,
              buffer_long_length_/2.);

  G4double first_buff_short_z = -buffer_long_length_/2. +
    (ring_drift_buffer_dist_/2.-cathode_opening_/2.) + buffer_ring_dist_/2.;

  G4UnionSolid* buff_holder_solid =
    new G4UnionSolid ("BUFF_HOLDER", buffer_long_solid, buffer_short_solid, 0,
                      G4ThreeVector(0., holder_long_y_/2. + holder_short_y_/2. -
                                    overlap_/2., first_buff_short_z));

  G4double posz;
  for (G4int j=1; j<num_buffer_rings_-1; j++) {
    posz = first_buff_short_z + j*buffer_ring_dist_;

    buff_holder_solid =
      new G4UnionSolid("BUFF_HOLDER", buff_holder_solid, buffer_short_solid, 0,
                       G4ThreeVector(0., holder_long_y_/2. + holder_short_y_/2.
                                     -overlap_/2., posz));
    }

  G4Box* buffer_last_solid =
    new G4Box("BUFF_LAST", holder_x_/2., holder_short_y_/2.+overlap_/2.,
              buffer_last_z_/2.);

  buff_holder_solid =
    new G4UnionSolid("BUFF_HOLDER", buff_holder_solid, buffer_last_solid, 0,
                     G4ThreeVector(0.,holder_long_y_/2. + holder_short_y_/2. -
                                   overlap_/2.,
                                   buffer_long_length_/2. - buffer_last_z_/2.));

  // DRIFT part.
  G4Box* drift_short_first_solid =
    new G4Box("DRIFT_SHORT", holder_x_/2., holder_short_y_/2. + overlap_/2.,
              drift_short_first_length_/2.);

  G4double first_drift_short_z =
    -drift_long_length_/2. + drift_short_first_length_/2.;

  G4Box* drift_long_solid =
    new G4Box("DRIFT_LONG", holder_x_/2., holder_long_y_/2.,
              drift_long_length_/2.);

  G4UnionSolid* drift_holder_solid =
    new G4UnionSolid ("DRIFT_HOLDER", drift_long_solid,
                      drift_short_first_solid, 0,
                      G4ThreeVector(0., holder_long_y_/2. + holder_short_y_/2. -
                                    overlap_/2., first_drift_short_z));

  G4Box* drift_short_solid =
    new G4Box("DRIFT_SHORT", holder_x_/2., holder_short_y_/2.+overlap_/2.,
              drift_short_length_/2.);

  for (G4int j=0; j<num_drift_rings_-1; j++) {
    posz = -drift_long_length_/2. + drift_short_first_length_ + drift_ring_dist_
      - drift_short_length_/2 + j*drift_ring_dist_;

    drift_holder_solid =
      new G4UnionSolid("DRIFT_HOLDER", drift_holder_solid, drift_short_solid, 0,
                       G4ThreeVector(0.,holder_long_y_/2. + holder_short_y_/2. -
                                     overlap_/2., posz));
    }

  // CATHODE part.
  // They are placed at the same z position as the cathode ring.
  G4Box* cathode_large_solid =
    new G4Box("CATHODE_LARGE", holder_x_/2., cathode_long_y_/2.,
              cathode_long_length_/2.);

  G4Box* cathode_short_solid =
    new G4Box("CATHODE_SHORT", holder_x_/2., holder_short_y_/2. + overlap_/2.,
              cathode_short_length_/2.);

  G4UnionSolid* cathode_holder_solid =
    new G4UnionSolid ("CATHODE_HOLDER", cathode_large_solid,
                      cathode_short_solid, 0,
                      G4ThreeVector(0., -(holder_short_y_/2. + overlap_/2. -
                                          cathode_long_y_/2),
                                    cathode_long_length_/2. -
                                    cathode_short_length_/2.));

  cathode_holder_solid =
    new G4UnionSolid("CATHODE_HOLDER", cathode_holder_solid,
                     cathode_short_solid, 0,
                     G4ThreeVector(0.,-(holder_short_y_/2. - cathode_long_y_/2),
                                    -(cathode_long_length_/2. -
                                      cathode_short_length_/2.)));

  // Union of the three parts
  G4double cathode_drift_ypos =
    -cathode_long_y_/2. - (holder_short_y_ - cathode_long_y_) -
    holder_long_y_/2.;
  G4double cathode_drift_zpos =
    - (cathode_long_length_/2. - cathode_short_length_ + drift_long_length_/2.);

  G4UnionSolid* full_stave_solid =
    new G4UnionSolid("STAVE", cathode_holder_solid, drift_holder_solid, 0,
                     G4ThreeVector(0., cathode_drift_ypos, cathode_drift_zpos));

  G4double cathode_buff_zpos =
    (cathode_long_length_/2. - cathode_short_length_ + buffer_long_length_/2.);

  full_stave_solid =
    new G4UnionSolid("STAVE", full_stave_solid, buff_holder_solid, 0,
                     G4ThreeVector(0., cathode_drift_ypos, cathode_buff_zpos));

  G4LogicalVolume* full_stave_logic =
    new G4LogicalVolume(full_stave_solid, pe500, "STAVE");

  SetLogicalVolume(full_stave_logic);


  /// Visibilities
  if (visibility_) {
    G4VisAttributes hold_col = nexus::LightGrey();
    full_stave_logic->SetVisAttributes(hold_col);
  } else {
    full_stave_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}
