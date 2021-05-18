// ----------------------------------------------------------------------------
// nexus | Next100InnerElements.h
//
// Inner elements of the NEXT-100 detector. They include the field cage,
// the energy and the tracking plane.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_INNER_ELEMENTS_H
#define NEXT100_INNER_ELEMENTS_H

#include <G4ThreeVector.hh>
#include <vector>
#include "GeometryBase.h"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class Next100FieldCage;
  class Next100EnergyPlane;
  class Next100TrackingPlane;

  class Next100InnerElements : public GeometryBase
  {

  public:
    ///Constructor
    Next100InnerElements();

    /// Destructor
    ~Next100InnerElements();

    /// Set the logical and physical volume that encloses the entire geometry
    void SetLogicalVolume(G4LogicalVolume*);
    void SetPhysicalVolume(G4VPhysicalVolume*);

    /// Return the relative position respect to the rest of NEXT100 geometry
    G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:

    const G4double gate_sapphire_wdw_distance_;
    const G4double gate_tracking_plane_distance_;


    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4Material* gas_;

    G4double pressure_;
    G4double temperature_;

    // Detector parts
    Next100FieldCage*     field_cage_;
    Next100EnergyPlane*   energy_plane_;
    Next100TrackingPlane* tracking_plane_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
