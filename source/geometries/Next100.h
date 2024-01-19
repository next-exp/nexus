// ----------------------------------------------------------------------------
// nexus | Next100.h
//
// Main class that constructs the geometry of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_H
#define NEXT100_H

#include "GeometryBase.h"

class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  class BoxPointSampler;
  class Next100Shielding;
  class Next100Vessel;
  class Next100Ics;
  class Next100InnerElements;
  class LSCHallA;

  class Next100: public GeometryBase
  {
  public:
    /// Constructor
    Next100();

    /// Destructor
    ~Next100();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns a point within a region projecting from a
    /// given point backwards along a line.
    G4ThreeVector ProjectToRegion(const G4String& region,
				  const G4ThreeVector& point,
				  const G4ThreeVector& dir) const;


  private:
    void BuildLab();
    void Construct();


  private:
    // Detector dimensions
    const G4double lab_size_; ///< Size of the air box containing the detector
    const G4double grid_thickness_; ///< Width of grids simulated as dielectric
    // const G4double ep_fc_distance_; ///< Distance between the surface of the
    // EP copper plate and the end of the staves/teflon panels/poly (a.k.a. FC)
    const G4double gate_tracking_plane_distance_, gate_sapphire_wdw_distance_;
    const G4double ics_ep_lip_width_ ; ///< width of step of the ICS bars in the
    /// energy plane side
    const G4double fc_displ_x_, fc_displ_y_;

    /// Specific vertex for AD_HOC region
    G4ThreeVector specific_vertex_;

    /// Whether or not to build LSC HallA.
    G4bool lab_walls_;

    /// Whether or not to print SiPM positions
    G4bool print_;

    /// Pointers to logical volumes
    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* buffer_gas_logic_;
    G4LogicalVolume* hallA_logic_;

    /// Detector parts
    LSCHallA* hallA_walls_;
    Next100Shielding* shielding_;
    Next100Vessel*    vessel_;
    Next100Ics*       ics_;
    Next100InnerElements* inner_elements_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    /// Origin of coordinates
    G4ThreeVector coord_origin_;
  };

} // end namespace nexus

#endif
