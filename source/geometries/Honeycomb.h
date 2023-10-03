// ----------------------------------------------------------------------------
// nexus | Honeycomb.h
//
// Support structure to the EP copper plate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HONEYCOMB_H
#define HONEYCOMB_H

#include "GeometryBase.h"

namespace nexus {
  class Honeycomb: public GeometryBase
  {
  public:
    /// Constructor
    Honeycomb();

    /// Destructor
    ~Honeycomb();

    void Construct();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Sets the logical volume where all inner elements are placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets the z position of the outer surface of the copper plate
    void SetEndOfCopperPlateZ(G4double z);



  private:

    G4LogicalVolume* mother_logic_;

    G4double end_of_EP_copper_plate_z_;

    // Relationships among beams
    G4double angle_, beam_dist_, beam_thickn_;
    G4double compl_angle_;
  };

  inline void Honeycomb::SetMotherLogicalVolume(G4LogicalVolume* mother_logic) {
    mother_logic_ = mother_logic;}

  inline void Honeycomb::SetEndOfCopperPlateZ(G4double z) {
    end_of_EP_copper_plate_z_ = z;}

} // end namespace nexus

#endif
