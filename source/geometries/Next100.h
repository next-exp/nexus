// ----------------------------------------------------------------------------
// nexus | Next100.h
//
// Main class that constructs the geometry of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_H
#define NEXT100_H

#include "BaseGeometry.h"

class G4LogicalVolume;
class G4GenericMessenger;

namespace nexus {class BoxPointSampler;}


namespace nexus {

  class BoxPointSampler;
  class Next100Shielding;
  class Next100Vessel;
  class Next100Ics;
  class Next100InnerElements;
  class LSCHallA;

  class Next100: public BaseGeometry
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
    const G4double lab_size_;          /// Size of the air box containing the detector

    // External diameter of nozzles and y positions
    const G4double nozzle_ext_diam_;
    const G4double up_nozzle_ypos_, central_nozzle_ypos_, down_nozzle_ypos_, bottom_nozzle_ypos_;

    // Pointers to logical volumes
    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* buffer_gas_logic_;
    G4LogicalVolume* hallA_logic_;

    // Detector parts
    LSCHallA* hallA_walls_;
    Next100Shielding* shielding_;
    Next100Vessel*    vessel_;
    Next100Ics*       ics_;
    Next100InnerElements* inner_elements_;

    BoxPointSampler* lab_gen_; ///< Vertex generator

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    /// Specific vertex for AD_HOC region
    G4double specific_vertex_X_;
    G4double specific_vertex_Y_;
    G4double specific_vertex_Z_;

    /// Position of gate in its mother volume
    G4double gate_zpos_in_vessel_;

    /// Whether or not to build LSC HallA.
    G4bool lab_walls_;
  };

} // end namespace nexus

#endif
